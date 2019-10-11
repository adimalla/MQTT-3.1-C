/**
 ******************************************************************************
 * @file    .c
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

#include "headers.h"


/* Function set connection to mqtt server */
int mqtt_broker_connect(int *fd, int port, char *server_address)
{

	int func_retval = 0;

	struct sockaddr_in server;

	/* Get client socket type */
	if( (*fd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		func_retval = -1;
	}

	server.sin_family = AF_INET;
	server.sin_port   = htons(port);

	server.sin_addr.s_addr = inet_addr(server_address);

	printf("length of address:%ld\n", strlen(server_address));

	/* Connect to MQTT server host machine */
	if( ( connect(*fd, (struct sockaddr*)&server, sizeof(server)) ) < 0)
	{
		func_retval = -2;
	}

	fcntl(*fd, F_SETFL, O_NONBLOCK);

	return func_retval;
}
