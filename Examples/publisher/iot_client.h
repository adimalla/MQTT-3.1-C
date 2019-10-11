/**
 ******************************************************************************
 * @file    iot_client.h
 * @author  Aditya Mall,
 * @brief   Example MQTT publish client, for mosquitto MQTT Broker
 *
 *  Info
 *          Only for testing, (POSIX compatible only.)
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2019 Aditya Mall </center></h2>
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */


#ifndef IOT_CLIENT_H_
#define IOT_CLIENT_H_

#include <string.h>


/******************************************************************************/
/*                                                                            */
/*                            Macro Defines                                   */
/*                                                                            */
/******************************************************************************/

#pragma pack(1)

#define MAX_ADDRESS_LENGTH  15



/******************************************************************************/
/*                                                                            */
/*                  Data Structures for Client Objects                        */
/*                                                                            */
/******************************************************************************/


/**/
typedef enum error_codes
{
	ERROR_CODE_NULL      =  0,
	FUNC_CODE_SUCCESS    =  1,
	CLIENT_INIT_ERROR    = -1,
	COMMAND_NO_ARGS      = -2,
	COMMAND_WRONG_ARGS   = -3,
	CLIENT_DESCRP_ERROR  = -4,
	SERVER_PORT_ERROR    = -5,
	SERVER_ADDRESS_ERROR = -6,
	CLIENT_DEINIT_ERROR  = -7,

}ClientRetVal;


/* */
typedef struct client
{
	/* Members */
	int  clientDescriptor;
	int  serverPortNumber;
	char *serverAddress;

	ClientRetVal returnValue;

	/* Methods */
	int (*connectServer)(int *descriptor, int portNumber, char *serverAddr);
	int (*getCommands)(int  argc, char **agrv);
	int (*write)(int descriptor, const void *buffer, size_t length);
	int (*read)(int descriptor, void *buffer, size_t length);
	int (*close)(int descriptor);

}IotClient;





/******************************************************************************/
/*                                                                            */
/*                           Function Prototypes                              */
/*                                                                            */
/******************************************************************************/


ClientRetVal clientInit(IotClient *client);

ClientRetVal checkInitializations(IotClient *client);

ClientRetVal clientDeinit(IotClient *client);




#endif /* MQTT_3_1_C_EXAMPLES_PUBLISHER_IOT_CLIENT_H_ */
