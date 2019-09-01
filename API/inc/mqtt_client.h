/**
 ******************************************************************************
 * @file    mqtt_client.h, file name will change
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

#ifndef MQQT_CLIENT_H_
#define MQQT_CLIENT_H_

/*
 * Standard Header and API Header files
 */
#include <stdint.h>




/******************************************************************************/
/*                                                                            */
/*                  Data Structures for MQTT Control Messages                 */
/*                                                                            */
/******************************************************************************/



/* @brief MQTT Fixed Header, common to all control messages */
typedef struct mqtt_header
{
    uint8_t retain_flag     : 1;  /*!< Message Retain Flag (LSB)                                               */
    uint8_t qos_level       : 2;  /*!< Quality of service Level (LSB)                                          */
    uint8_t dup_flag        : 1;  /*!< Duplicate Flag (LSB)                                                    */
	uint8_t message_type    : 4;  /*!< Message Type (MSB)                                                      */
	uint8_t message_length;       /*!< Length of MQTT Message (Remaining Length, excludes fixed header length) */

}mqtt_header_t;



/* @brief MQTT CONNECT structures */

/* Connect Flags bit fields */
typedef struct mqtt_connect_flags
{
	uint8_t reserved        : 1;
	uint8_t clean_session   : 1;
	uint8_t will_flag       : 1;
	uint8_t will_qos        : 1;
	uint8_t will_retain     : 1;
	uint8_t password_flag   : 1;
	uint8_t username_flag   : 1;

}mqtt_connect_flags_t;


/* Main MQTT Connect Structure */
typedef struct mqtt_connect
{
	mqtt_header_t        fixed_header;          /*!< MQTT Fixed Header         */
	uint16_t             protocol_name_length;  /*!< MQTT Protocol Name Length */
	char                 protocol_name[6];      /*!< MQTT Protocol Name        */
	uint8_t              protocol_version;      /*!< MQTT Protocol Version     */
	mqtt_connect_flags_t connect_flags;         /*!< Connect Message Flags     */
	uint16_t             keep_alive_value;      /*!< Client Keep Alive Value   */
	uint16_t             client_id_length;      /*!< Client ID Length          */
	char                 client_id[23];         /*!< Client ID                 */

}mqtt_connect_t;





/******************************************************************************/
/*                                                                            */
/*                       API Function Prototypes                              */
/*                                                                            */
/******************************************************************************/



#endif /* MQQT_CLIENT_H_ */
