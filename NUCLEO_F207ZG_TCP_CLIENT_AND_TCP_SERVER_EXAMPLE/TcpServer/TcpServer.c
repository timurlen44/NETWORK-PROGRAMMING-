/*
 * TcpServer.c
 *
 *  Created on: Jul 4, 2025
 *      Author: ayoff
 */

#include "TcpServer.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "string.h"

#if LWIP_TCP

typedef enum TcpServerConnectionStates //TCP Server Connection States
{
	TCP_SERVER_CONNECTION_STATE_IS_NONE = 0,
	TCP_SERVER_CONNECTION_STATE_IS_ACCEPTED,
	TCP_SERVER_CONNECTION_STATE_IS_RECEIVED,
	TCP_SERVER_CONNECTION_STATE_IS_CLOSED
} TcpServerConnectionStates_e;

/**
 * @structure
 * @brief structure for maintaing connection infos to be passed as argument to LwIP callbacks
 */
typedef struct
{
	uint8_t state; 	     //!< current connection state
	struct tcp_pcb *pcb; //!< pointer on the current tcp_pcb
	struct pbuf *p_rx; 	 //!< pointer on the received pbuf
	uint16_t receivedDataBufferCounter;
	uint32_t lastRecvTime_u32;
} TcpServerArguments_t;

static struct tcp_pcb *g_ethernetTcpServerPcb;
TcpServerArguments_t *g_tcpServerArguments = NULL;
static uint8_t tcpServerReceivedDataBuffer[1000] = { 0 }; //!< hold data that is received from tcp client
static void (*handleReceivedTcpServerPacket)(const uint8_t* const tcpServerDataBuffer, const uint16_t receivedDataLength_u16) = NULL; //!< Function Pointer that is called whenever tcp server receives whole tcp client's packet

/**
 * @brief  This function is the implementation of tcp_accept LwIP callback
 * @param  arg: not used
 * @param  newpcb: pointer on tcp_pcb struct for the newly created tcp connection
 * @param  err: not used
 * @retval err_t: error status
 */
static err_t tcp_server_accept(void* arg, struct tcp_pcb* newpcb, err_t err);

/**
 * @brief  This function is the implementation for tcp_recv LwIP callback
 * @param  arg: pointer on a argument for the tcp_pcb connection
 * @param  tpcb: pointer on the tcp_pcb connection
 * @param  pbuf: pointer on the received pbuf
 * @param  err: error information regarding the reveived pbuf
 * @retval err_t: error code
 */
static err_t tcp_server_recv(void* arg, struct tcp_pcb* tpcb, struct pbuf* p, err_t err);

/**
 * @brief  This function implements the tcp_err callback function (called
 *         when a fatal tcp_connection error occurs.
 * @param  arg: pointer on argument parameter
 * @param  err: not used
 * @retval None
 */
static void tcp_server_error(void* arg, err_t err);

/**
 * @brief  This function implements the tcp_poll LwIP callback function
 * @param  arg: pointer on argument passed to callback
 * @param  tpcb: pointer on the tcp_pcb for the current tcp connection
 * @retval err_t: error code
 */
static err_t tcp_server_poll(void* arg, struct tcp_pcb* tpcb);

/**
 * @brief  This functions closes the tcp connection
 * @param  tcp_pcb: pointer on the tcp connection
 * @param  es: pointer on echo_state structure
 * @retval None
 * @warning call this function just for one time
 */
static void tcp_server_connection_close(struct tcp_pcb* tpcb, TcpServerArguments_t* tcpServerArguments);

/**
 * @brief this function fils the received data buffer
 * @param  tcp_pcb: pointer on the tcp connection
 * @retval none
 */
static void fillTcpServerReceivedDataBuffer(struct tcp_pcb* tpcb, TcpServerArguments_t* tcpServerArguments);

bool initEthernetTcpServer(uint16_t port_u16, void (*handleReceivedTcpServerPacketFuncPtr)(const uint8_t* const tcpServerDataBuffer, const uint16_t receivedDataLength_u16))
{
	bool tcpServerInitStatus = false;

	if (handleReceivedTcpServerPacketFuncPtr == NULL)
	{
		return false;
	}

	handleReceivedTcpServerPacket = handleReceivedTcpServerPacketFuncPtr;

	if (g_ethernetTcpServerPcb != NULL)
	{
		return false; // !!! do not call this function twice if you wanna change port restart and give right port !!!
	}

	g_ethernetTcpServerPcb = tcp_new (); // create new tcp pcb

	if (g_ethernetTcpServerPcb != NULL)
	{
		err_t err = tcp_bind (g_ethernetTcpServerPcb, IP_ADDR_ANY, port_u16);
		if (err == ERR_OK)
		{
			tcpServerInitStatus = true;
			g_ethernetTcpServerPcb = tcp_listen(g_ethernetTcpServerPcb); // //start tcp listening for echo_pcb
			tcp_accept (g_ethernetTcpServerPcb, tcp_server_accept); //initialize LwIP tcp_accept callback function
		}
		else
		{
			memp_free (MEMP_TCP_PCB, g_ethernetTcpServerPcb); //deallocate the pcb
		}
	}

	return tcpServerInitStatus;
}

static err_t tcp_server_accept(void* arg, struct tcp_pcb* newpcb, err_t err)
{
	LWIP_UNUSED_ARG(arg);
	LWIP_UNUSED_ARG(err);

	err_t ret_err;

	if (g_tcpServerArguments != NULL)
	{
		tcp_server_connection_close (newpcb, NULL);
		return ERR_OK;
	}

	tcp_setprio (newpcb, TCP_PRIO_MAX); //set priority for the newly accepted tcp connection newpcb

	TcpServerArguments_t *tcpServerArguments = (TcpServerArguments_t*) mem_malloc (sizeof(TcpServerArguments_t)); //allocate structure tcpServerArguments to maintain tcp connection informations
	if (tcpServerArguments != NULL)
	{
		tcpServerArguments->state = TCP_SERVER_CONNECTION_STATE_IS_ACCEPTED;
		tcpServerArguments->pcb = newpcb;
		tcpServerArguments->p_rx = NULL;
		tcpServerArguments->lastRecvTime_u32 = HAL_GetTick ();
		tcpServerArguments->receivedDataBufferCounter = 0;
		g_tcpServerArguments = tcpServerArguments;

		tcp_arg (newpcb, tcpServerArguments); //pass newly allocated es structure as argument to newpcb
		tcp_recv (newpcb, tcp_server_recv); //initialize lwip tcp_recv callback function for newpcb
		tcp_err (newpcb, tcp_server_error); //initialize lwip tcp_err callback function for newpcb
		tcp_poll (newpcb, tcp_server_poll, 1); //initialize lwip tcp_poll callback function for newpcb
		ret_err = ERR_OK;
	}
	else
	{
		tcp_server_connection_close (newpcb, tcpServerArguments); // close tcp connection
		ret_err = ERR_MEM; //return memory error
	}
	return ret_err;
}

static err_t tcp_server_recv(void* arg, struct tcp_pcb* tpcb, struct pbuf* p, err_t err)
{
	err_t ret_err;

	TcpServerArguments_t *tcpServerArguments = (TcpServerArguments_t*) arg;
	tcpServerArguments->lastRecvTime_u32 = HAL_GetTick ();

	if (p == NULL) //if we receive an empty tcp frame from client => close connection
	{
		tcpServerArguments->state = TCP_SERVER_CONNECTION_STATE_IS_CLOSED; //remote host closed connection
		if (tcpServerArguments->p_rx == NULL)
		{
			uint16_t tcpServerReceivedData_u16 = tcpServerArguments->receivedDataBufferCounter;
			handleReceivedTcpServerPacket (tcpServerReceivedDataBuffer, tcpServerReceivedData_u16);
			tcp_server_connection_close (tpcb, tcpServerArguments); //we're done sending, close connection

		}
		else
		{
			fillTcpServerReceivedDataBuffer (tpcb, tcpServerArguments);
		}
		ret_err = ERR_OK;
	}
	else //else : a non empty frame was received from client but for some reason err != ERR_OK
	{
		if (err != ERR_OK)
		{

			if (p != NULL) // free received pbuf
			{
				tcpServerArguments->p_rx = NULL;
				pbuf_free (p);
			}
			ret_err = err;
		}
		else
		{

			if (tcpServerArguments->state == TCP_SERVER_CONNECTION_STATE_IS_ACCEPTED)
			{
				tcpServerArguments->state = TCP_SERVER_CONNECTION_STATE_IS_RECEIVED; //first data chunk in p->payload
				tcpServerArguments->p_rx = p; //store reference to incoming pbuf (chain)
				fillTcpServerReceivedDataBuffer (tpcb, tcpServerArguments);
				ret_err = ERR_OK;
			}
			else
			{

				if (tcpServerArguments->state == TCP_SERVER_CONNECTION_STATE_IS_RECEIVED)
				{
					if (tcpServerArguments->p_rx == NULL) //more data received from client and previous data has been already sent
					{
						tcpServerArguments->p_rx = p;
						fillTcpServerReceivedDataBuffer (tpcb, tcpServerArguments);
					}
					else
					{

						struct pbuf *ptr = tcpServerArguments->p_rx; //chain pbufs to the end of what we recv'ed previous
						pbuf_chain (ptr, p);
					}
					ret_err = ERR_OK;
				}
				else //data received when connection already closed
				{
					tcp_recved (tpcb, p->tot_len); //Acknowledge data reception
					tcpServerArguments->p_rx = NULL; //free pbuf and do nothing
					pbuf_free (p);
					ret_err = ERR_OK;
				}
			}
		}
	}
	return ret_err;
}

static void tcp_server_error(void* arg, err_t err)
{
	LWIP_UNUSED_ARG(err);
	TcpServerArguments_t *tcpServerArguments = (TcpServerArguments_t*) arg;
	if (tcpServerArguments != NULL)
	{
		mem_free (tcpServerArguments); // free es structure
	}
}

static err_t tcp_server_poll(void* arg, struct tcp_pcb* tpcb)
{
	err_t ret_err;
	TcpServerArguments_t *tcpServerArguments = (TcpServerArguments_t*) arg;
	if (tcpServerArguments != NULL)
	{
		if (tcpServerArguments->p_rx == NULL)
		{
			uint32_t currentTime_u32 = HAL_GetTick ();
			if ((currentTime_u32 - tcpServerArguments->lastRecvTime_u32) >= 5000)
			{
				tcpServerArguments->state = TCP_SERVER_CONNECTION_STATE_IS_CLOSED;
				uint16_t tcpServerReceivedData_u16 = tcpServerArguments->receivedDataBufferCounter;
				handleReceivedTcpServerPacket (tcpServerReceivedDataBuffer, tcpServerReceivedData_u16);
				tcp_server_connection_close (tpcb, tcpServerArguments); // close tcp connection
			}

		}
		ret_err = ERR_OK;
	}
	else
	{
		tcp_abort (tpcb); //nothing to be done
		ret_err = ERR_ABRT;
	}
	return ret_err;
}

static void tcp_server_connection_close(struct tcp_pcb* tpcb, TcpServerArguments_t* tcpServerArguments)
{
	tcp_arg (tpcb, NULL); //remove all callbacks
	tcp_sent (tpcb, NULL);
	tcp_recv (tpcb, NULL);
	tcp_err (tpcb, NULL);
	tcp_poll (tpcb, NULL, 0);

	if (tcpServerArguments != NULL) //delete es structure
	{
		g_tcpServerArguments = NULL;
		mem_free (tcpServerArguments);
		tcp_abort (tpcb); //close tcp connection
	}
	else
	{
		tcp_close (tpcb); //close tcp connection
	}

}

static void fillTcpServerReceivedDataBuffer(struct tcp_pcb* tpcb, TcpServerArguments_t* tcpServerArguments)
{
	struct pbuf *pbuf_ptr = NULL;

	while ((tcpServerArguments->p_rx != NULL) && (tcpServerArguments->p_rx->len))
	{
		pbuf_ptr = tcpServerArguments->p_rx; //get pointer on pbuf from es structure

		u16_t copyLength_u16 =
				((pbuf_ptr->len + tcpServerArguments->receivedDataBufferCounter) < sizeof(tcpServerReceivedDataBuffer)) ? pbuf_ptr->len : (sizeof(tcpServerReceivedDataBuffer) - tcpServerArguments->receivedDataBufferCounter);

		memcpy ((tcpServerReceivedDataBuffer + (tcpServerArguments->receivedDataBufferCounter)), pbuf_ptr->payload, copyLength_u16);
		tcpServerArguments->receivedDataBufferCounter += copyLength_u16;

		u16_t pbuf_len = pbuf_ptr->len;
		tcpServerArguments->p_rx = pbuf_ptr->next; //continue with next pbuf in chain (if any)

		if (tcpServerArguments->p_rx != NULL)
		{
			pbuf_ref (tcpServerArguments->p_rx); //increment reference count for tcpServerArguments->p
		}

		pbuf_free (pbuf_ptr); //free pbuf: will free pbufs up to es->p (because es->p has a reference count > 0)
		tcp_recved (tpcb, pbuf_len); //Update tcp window size to be advertized : should be called when received data (with the amount plen) has been processed by the application layer

	}
}

#endif /* LWIP_TCP */
