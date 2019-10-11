/**
 ******************************************************************************
 * @file    iot_client.c
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


/* header files */
#include "iot_client.h"
#include <string.h>
#include <stdlib.h>


ClientRetVal clientInit(IotClient *client)
{
	ClientRetVal errorCode = FUNC_CODE_SUCCESS;

	/* Check param error */
	if(client == NULL)
	{
		errorCode = CLIENT_INIT_ERROR;
	}
	else
	{
		/* Initialize members */
		client->serverPortNumber = 0;

		client->serverAddress = malloc(sizeof(char) * MAX_ADDRESS_LENGTH);

		client->returnValue = FUNC_CODE_SUCCESS;

		memset(client->serverAddress, 0, sizeof(MAX_ADDRESS_LENGTH));
	}

	return errorCode;
}



ClientRetVal checkInitializations(IotClient *client)
{
	ClientRetVal errorCode = FUNC_CODE_SUCCESS;

	/* Check param error */
	if(client == NULL)
	{
		errorCode = CLIENT_INIT_ERROR;
	}
	else
	{
		/* Check of structure member values */
		if(client->serverPortNumber <= 0)
			errorCode = SERVER_PORT_ERROR;

		else if( strlen(client->serverAddress) > MAX_ADDRESS_LENGTH)
		{
			errorCode = SERVER_ADDRESS_ERROR;
		}

		/* Close client descriptor */
		client->close(client->clientDescriptor);

	}

	return errorCode;
}



ClientRetVal clientDeinit(IotClient *client)
{
	ClientRetVal errorCode = FUNC_CODE_SUCCESS;

	/* Check param error */
	if(client == NULL)
	{
		errorCode = CLIENT_DEINIT_ERROR;
	}
	else
	{
		/* De-initialize members */
		client->serverPortNumber = 0;

		free(client->serverAddress);
		client->serverAddress = NULL;

		client->returnValue = ERROR_CODE_NULL;
	}

	return errorCode;
}
