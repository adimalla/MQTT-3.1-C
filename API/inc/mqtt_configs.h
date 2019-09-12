/**
 ******************************************************************************
 * @file    mqtt_configs.h, file name will change
 * @author  Aditya Mall,
 * @brief   Example MQTT publish client, for mosquitto MQTT Broker
 *
 *  Info
 *          API
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
#ifndef MQTT_CONFIGS_H_
#define MQTT_CONFIGS_H_



/******************************************************************************/
/*                                                                            */
/*                  MQTT Message Macros and Defines                           */
/*                                                                            */
/******************************************************************************/


/* @brief Generic Defines */
#define ENABLE  1
#define DISABLE 0
#define GCC     1


/* @brief MQTT defines */
#define FIXED_HEADER_LENGTH   2         /*!< Size of MQTT Fixed Header                                    */
#define PROTOCOL_NAME_LENGTH  6         /*!< Size of MQTT Protocol                                        */
#define PROTOCOL_NAME         "MQIsdp"  /*!< MQTT Protocol name                                           */
#define PROTOCOL_VERSION      3         /*!< MQTT Protocol version                                        */
#define CLIENT_ID_LENGTH      23        /*!< MQTT client ID, fixed by mosquitto broker, do not change!    */
#define USER_NAME_LENGTH      15        /*!< MQTT username size, variable can be changed by user          */
#define PASSWORD_LENGTH       10        /*!< MQTT passwords size, variable can be changed by user         */
#define TOPIC_LENGTH          30        /*!< MQTT topic length size, variable can be changed by user      */
#define MESSAGE_LENGTH        100       /*!< MQTT message/payload length, variable can be changed by user */


#endif /* INC_MQTT_CONFIGS_H_ */
