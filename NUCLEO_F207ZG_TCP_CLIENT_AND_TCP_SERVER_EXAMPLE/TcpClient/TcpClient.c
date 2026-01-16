/*
 * TcpClient.c
 *
 *  Created on: Jul 4, 2025
 *      Author: Muhammed Emin TANIK
 */

#include "stdatomic.h"
#include "TcpClient.h"
#include "main.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "lwip/memp.h"
#include <string.h>
#include "main.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "lwip/memp.h"
#include <string.h>

#if LWIP_TCP

/**
 * @enum
 * @brief states last connection status
 */
typedef enum
{
	LAST_CONNECETION_STATUS_NONE = 0,
	LAST_CONNECETION_STATUS_NOT_CONNECTED_YET,
	LAST_CONNECETION_STATUS_CONNECTED,
	LAST_CONNECETION_STATUS_CLOSED
} last_connection_status_e;

/**
 * @enum
 * @brief Tcp Client States
 */
enum tcp_client_connection_states
{
	TCP_CLIENT_CONNECTION_STATE_IS_NOT_CONNECTED = 0,
	TCP_CLIENT_CONNECTION_STATE_IS_CONNECTED,
	TCP_CLIENT_CONNECTION_STATE_IS_RECEIVED,
	TCP_CLIENT_CONNECTION_STATE_IS_CLOSING,
};

/**
 * @struct
 * @brief structure to be passed as argument to the tcp callbacks
 */
struct TcpClientArguments_t
{
	enum tcp_client_connection_states state; // connection status
	struct tcp_pcb *pcb;          // pointer on the current tcp_pcb
	struct pbuf *p_tx;            // pointer on pbuf to be transmitted
	uint16_t poll_counter;

};

struct tcp_pcb *g_ethernetTcpClientPcb = NULL;
ip_addr_t g_destIPaddr = { 0 };
uint16_t g_destinationPort_u16 = { 0 };
static volatile last_connection_status_e g_last_connection_status = LAST_CONNECETION_STATUS_NONE;
static struct TcpClientArguments_t *g_tcpClientArguments = NULL;

/**
 * @brief This function is used to close the tcp connection with server
 * @param tpcb: tcp connection control block
 * @param es: pointer on TcpClientArguments_t structure
 * @retval None
 */
static void tcp_client_connection_close(struct tcp_pcb* tpcb, struct TcpClientArguments_t* tcpClientArguments);

/**
 * @brief  This function implements the tcp_poll callback function
 * @param  arg: pointer on argument passed to callback
 * @param  tpcb: tcp connection control block
 * @retval err_t: error code
 */
static err_t tcp_client_poll(void* arg, struct tcp_pcb* tpcb);

/**
 * @brief  This function implements the tcp_sent LwIP callback (called when ACK
 *         is received from remote host for sent data)
 * @param  arg: pointer on argument passed to callback
 * @param  tcp_pcb: tcp connection control block
 * @param  len: length of data sent
 * @retval err_t: returned error code
 */
static err_t tcp_client_sent(void* arg, struct tcp_pcb* tpcb, u16_t len);

/**
 * @brief function used to send data
 * @param  tpcb: tcp control block
 * @param  es: pointer on structure of type TcpClientArguments_t containing info on data
 *             to be sent
 * @retval None
 */
static void tcp_client_send(struct tcp_pcb* tpcb, struct TcpClientArguments_t* tcpClientArguments);

/**
 * @brief Function called when TCP connection established
 * @param tpcb: pointer on the connection contol block
 * @param err: when connection correctly established err should be ERR_OK
 * @retval err_t: returned error
 */
static err_t tcp_client_connected(void* arg, struct tcp_pcb* tpcb, err_t err);

/**
 * @function
 * @brief Function called when TCP connection has an error
 * @param arg: pointer on argument passed to callback
 * @param err: when connection correctly established err should be ERR_OK
 * @retval none
 */
static void tcp_client_error_callback(void* arg, err_t err);

/**
 * @function
 * @brief if someting went wrong this function will be called by tcp_client_error_callback
 * @retval none
 */
static void resendEthernetTcpClientData(void* arg);

static err_t tcp_client_connected(void* arg, struct tcp_pcb* tpcb, err_t err)
{
	struct TcpClientArguments_t *tcpClientArguments = arg;

	if (err == ERR_OK)
	{
		if (tcpClientArguments != NULL)
		{
			g_last_connection_status = LAST_CONNECETION_STATUS_CONNECTED;
			tcpClientArguments->state = TCP_CLIENT_CONNECTION_STATE_IS_CONNECTED;
			if (tcpClientArguments->p_tx)
			{
				tcp_client_send (tpcb, tcpClientArguments); // send data
				return ERR_OK;
			}
		}
		else
		{
			tcp_client_connection_close (tpcb, tcpClientArguments); // close connection
			return ERR_MEM; // return memory allocation error
		}
	}
	else
	{
		tcp_client_connection_close (tpcb, tcpClientArguments); // close connection
	}
	return err;
}

static void tcp_client_send(struct tcp_pcb* tpcb, struct TcpClientArguments_t* tcpClientArguments)
{
	struct pbuf *pbuf_ptr;
	err_t wr_err = ERR_OK;

	while ((wr_err == ERR_OK) && (tcpClientArguments->p_tx != NULL) && (tcpClientArguments->p_tx->len <= tcp_sndbuf(tpcb)))
	{
		pbuf_ptr = tcpClientArguments->p_tx; // get pointer on pbuf from es structure
		wr_err = tcp_write (tpcb, pbuf_ptr->payload, pbuf_ptr->len, 1); // enqueue data for transmission

		if (wr_err == ERR_OK)
		{
			tcpClientArguments->p_tx = pbuf_ptr->next; // continue with next pbuf in chain (if any)

			if (tcpClientArguments->p_tx != NULL)
			{
				pbuf_ref (tcpClientArguments->p_tx); // increment reference count for tcpClientArguments->p
			}

			pbuf_free (pbuf_ptr); // free pbuf: will free pbufs up to tcpClientArguments->p (because tcpClientArguments->p has a reference count > 0)
		}
		else
			if (wr_err == ERR_MEM)
			{
				tcpClientArguments->p_tx = pbuf_ptr; // we are low on memory, try later, defer to poll
			}
			else
			{
				// other problem ?? //
			}
	}
}

static err_t tcp_client_poll(void* arg, struct tcp_pcb* tpcb)
{
	struct TcpClientArguments_t *tcpClientArguments = (struct TcpClientArguments_t*) arg;
	err_t ret_err;

	tcpClientArguments->poll_counter++;

	if (tcpClientArguments != NULL)
	{
		if (tcpClientArguments->p_tx != NULL)
		{
			tcp_connect (g_ethernetTcpClientPcb, &g_destIPaddr, g_destinationPort_u16, tcp_client_connected); // there is a remaining pbuf (chain) , try to send data
		}
		else
		{
			tcp_client_connection_close (tpcb, tcpClientArguments); // close tcp connection
		}
		ret_err = ERR_OK;
	}
	else // nothing to be done
	{
		mem_free (tcpClientArguments);
		g_ethernetTcpClientPcb = NULL;
		g_tcpClientArguments = NULL;
		g_last_connection_status = LAST_CONNECETION_STATUS_CLOSED;
		tcp_abort (tpcb);
		ret_err = ERR_ABRT;
	}
	return ret_err;
}

static err_t tcp_client_sent(void* arg, struct tcp_pcb* tpcb, u16_t len)
{
	LWIP_UNUSED_ARG(len);

	struct TcpClientArguments_t *tcpClientArguments = (struct TcpClientArguments_t*) arg;

	if (tcpClientArguments->p_tx != NULL)
	{
		tcp_client_send (tpcb, tcpClientArguments); // still got pbufs to send
	}

	if (tcpClientArguments->p_tx == NULL)
	{
		tcp_client_connection_close (tpcb, tcpClientArguments);

	}

	return ERR_OK;
}

static void tcp_client_connection_close(struct tcp_pcb* tpcb, struct TcpClientArguments_t* tcpClientArguments)
{

	tcp_sent (tpcb, NULL); // // remove callbacks
	tcp_poll (tpcb, NULL, 0);
	tcp_err (tpcb, NULL);

	if (tcpClientArguments != NULL)
	{
		if (tcpClientArguments->p_tx != NULL)
		{
			pbuf_free (tcpClientArguments->p_tx);
		}

		mem_free (tcpClientArguments);
	}

	tcp_close (tpcb); // close tcp connection
	g_ethernetTcpClientPcb = NULL;
	g_tcpClientArguments = NULL;
	g_last_connection_status = LAST_CONNECETION_STATUS_CLOSED;

}

static void tcp_client_error_callback(void* arg, err_t err)
{
	g_ethernetTcpClientPcb = NULL;
	g_tcpClientArguments = NULL;
	resendEthernetTcpClientData (arg);
}

static void resendEthernetTcpClientData(void* arg)
{
	struct TcpClientArguments_t *tcpClientArguments = (struct TcpClientArguments_t*) arg;

	if (g_ethernetTcpClientPcb == NULL)
	{
		g_ethernetTcpClientPcb = tcp_new (); // create new tcp pcb

		if (g_ethernetTcpClientPcb == NULL)
		{
			if (tcpClientArguments->p_tx != NULL)
			{
				pbuf_free (tcpClientArguments->p_tx);
			}

			mem_free (g_tcpClientArguments);
			return; // couldn't created a pcb
		}

		tcpClientArguments->pcb = g_ethernetTcpClientPcb;
		tcpClientArguments->state = TCP_CLIENT_CONNECTION_STATE_IS_NOT_CONNECTED;
		tcpClientArguments->poll_counter = 0;
		g_tcpClientArguments = tcpClientArguments;

		if (tcpClientArguments->p_tx)
		{
			tcp_arg (g_ethernetTcpClientPcb, tcpClientArguments); // pass newly allocated es structure as argument to tpcb
			tcp_sent (g_ethernetTcpClientPcb, tcp_client_sent); // initialize LwIP tcp_sent callback function
			tcp_poll (g_ethernetTcpClientPcb, tcp_client_poll, 1); // initialize LwIP tcp_poll callback function
			tcp_err (g_ethernetTcpClientPcb, tcp_client_error_callback);
			g_last_connection_status = LAST_CONNECETION_STATUS_NOT_CONNECTED_YET;
			tcp_connect (g_ethernetTcpClientPcb, &g_destIPaddr, g_destinationPort_u16, tcp_client_connected);
		}
		else
		{
			tcp_client_connection_close (g_ethernetTcpClientPcb, tcpClientArguments); // Out of memory
			return; // return ERR_MEM
		}
	}
	if (g_ethernetTcpClientPcb != NULL)
	{
		if ((g_last_connection_status == LAST_CONNECETION_STATUS_CLOSED) || (g_last_connection_status == LAST_CONNECETION_STATUS_NONE))
		{
			g_last_connection_status = LAST_CONNECETION_STATUS_NOT_CONNECTED_YET;
			tcp_connect (g_ethernetTcpClientPcb, &g_destIPaddr, g_destinationPort_u16, tcp_client_connected);
		}
		else
		{
			if (g_last_connection_status == LAST_CONNECETION_STATUS_NOT_CONNECTED_YET)
			{
				tcp_connect (g_ethernetTcpClientPcb, &g_destIPaddr, g_destinationPort_u16, tcp_client_connected);
			}
		}

	}
	return;

}

bool initEthernetTcpClient(const uint8_t destinationIpAddr[4], const uint16_t destinationPort_u16)
{
	setEthernetTcpClientDestinationIpAddrAndPort (destinationIpAddr, destinationPort_u16);
	return true;
}

bool isEthernetTcpClientAvailableForSending(void)
{
	if ((g_last_connection_status == LAST_CONNECETION_STATUS_CLOSED) || (g_last_connection_status == LAST_CONNECETION_STATUS_NONE))
	{
		return true;
	}

	return false;
}

bool sendEthernetTcpClientData(const uint8_t* const data, const uint16_t size)
{

	if ((size == 0) || (data == NULL))
		return false;

	if (g_ethernetTcpClientPcb == NULL)
	{
		g_ethernetTcpClientPcb = tcp_new (); // create new tcp pcb

		if (g_ethernetTcpClientPcb == NULL)
			return false; // couldn't created a pcb

		tcp_nagle_disable(g_ethernetTcpClientPcb); // it prevent bundling the small packets, thus i can send my data ever 1 ms

		struct TcpClientArguments_t *tcpClientArguments = (struct TcpClientArguments_t*) mem_calloc (1, sizeof(struct TcpClientArguments_t)); // allocate structure es to maintain tcp connection informations

		if (tcpClientArguments == NULL)
			return false;

		tcpClientArguments->pcb = g_ethernetTcpClientPcb;
		tcpClientArguments->state = TCP_CLIENT_CONNECTION_STATE_IS_CLOSING;
		tcpClientArguments->p_tx = pbuf_alloc (PBUF_TRANSPORT, size, PBUF_POOL); // allocate pbuf
		tcpClientArguments->poll_counter = 0;
		g_tcpClientArguments = tcpClientArguments;

		if (tcpClientArguments->p_tx)
		{
			pbuf_take (tcpClientArguments->p_tx, data, size); // copy data to pbuf
			tcp_arg (g_ethernetTcpClientPcb, tcpClientArguments); // pass newly allocated es structure as argument to tpcb
			tcp_sent (g_ethernetTcpClientPcb, tcp_client_sent); // initialize LwIP tcp_sent callback function
			tcp_poll (g_ethernetTcpClientPcb, tcp_client_poll, 1); // initialize LwIP tcp_poll callback function
			tcp_err (g_ethernetTcpClientPcb, tcp_client_error_callback);
			tcpClientArguments->state = TCP_CLIENT_CONNECTION_STATE_IS_NOT_CONNECTED;
			g_last_connection_status = LAST_CONNECETION_STATUS_NOT_CONNECTED_YET;
			tcp_connect (g_ethernetTcpClientPcb, &g_destIPaddr, g_destinationPort_u16, tcp_client_connected);
		}
		else
		{
			tcp_client_connection_close (g_ethernetTcpClientPcb, tcpClientArguments); // Out of memory
			return false; // return ERR_MEM
		}
	}
	else
	{

		if ((g_last_connection_status == LAST_CONNECETION_STATUS_CLOSED) || (g_last_connection_status == LAST_CONNECETION_STATUS_NONE))
		{
			g_last_connection_status = LAST_CONNECETION_STATUS_NOT_CONNECTED_YET;
			tcp_connect (g_ethernetTcpClientPcb, &g_destIPaddr, g_destinationPort_u16, tcp_client_connected);
		}
		else
		{
			if (g_last_connection_status == LAST_CONNECETION_STATUS_NOT_CONNECTED_YET)
			{
				tcp_connect (g_ethernetTcpClientPcb, &g_destIPaddr, g_destinationPort_u16, tcp_client_connected);
			}

		}

	}
	return true;
}

bool setEthernetTcpClientDestinationIpAddrAndPort(const uint8_t destinationIpAddr[4], const uint16_t destinationPort)
{
	IP_ADDR4( &g_destIPaddr, destinationIpAddr[0], destinationIpAddr[1], destinationIpAddr[2], destinationIpAddr[3]);
	g_destinationPort_u16 = destinationPort;
	return true;
}

#endif // LWIP_TCP //
