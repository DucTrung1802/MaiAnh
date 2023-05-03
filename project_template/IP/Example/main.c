/********************************************************************************************************/
/*
 * @file    IP/Example/main.c
 * @version $Rev:: 4869         $
 * @date    $Date:: 2020-08-05 #$
 * @brief   Main program.
 *************************************************************************************************************
 * @attention
 *
 * Firmware Disclaimer Information
 *
 * 1. The customer hereby acknowledges and agrees that the program technical documentation, including the
 *    code, which is supplied by Holtek Semiconductor Inc., (hereinafter referred to as "HOLTEK") is the
 *    proprietary and confidential intellectual property of HOLTEK, and is protected by copyright law and
 *    other intellectual property laws.
 *
 * 2. The customer hereby acknowledges and agrees that the program technical documentation, including the
 *    code, is confidential information belonging to HOLTEK, and must not be disclosed to any third parties
 *    other than HOLTEK and the customer.
 *
 * 3. The program technical documentation, including the code, is provided "as is" and for customer reference
 *    only. After delivery by HOLTEK, the customer shall use the program technical documentation, including
 *    the code, at their own risk. HOLTEK disclaims any expressed, implied or statutory warranties, including
 *    the warranties of merchantability, satisfactory quality and fitness for a particular purpose.
 *
 * <h2><center>Copyright (C) Holtek Semiconductor Inc. All rights reserved</center></h2>
 ************************************************************************************************************/
// <<< Use Configuration Wizard in Context Menu >>>

/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"

#include "ht32_board.h"

#include "stdlib.h"

#include "string.h"

/** @addtogroup Project_Template Project Template
 * @{
 */

/** @addtogroup IP_Examples IP
 * @{
 */

/** @addtogroup Example
 * @{
 */

/* Settings ------------------------------------------------------------------------------------------------*/



/* Private types -------------------------------------------------------------------------------------------*/
struct BC660K {
  /* Debug */
  char * log_content;
	
	/* Command */
	vu32 command_timer;
	char *command;
	u16 module_buffer_index;
	char *module_buffer;
} BC660K;

enum StatusType {
		STATUS_SUCCESS = 0,
		STATUS_ERROR,
		STATUS_TIMEOUT,
		STATUS_BAD_PARAMETERS,
		STATUS_UNKNOWN
} StatusType;

/* Private constants ---------------------------------------------------------------------------------------*/
const char *SUCCESS_COMMAND_SIGN[] = { "\r\n\r\n", "OK\r\n" };
const char *ERROR_COMMAND_SIGN[] = { "ERROR" };

#define CA_CERT "-----BEGIN CERTIFICATE-----\n\
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n\
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n\
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n\
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n\
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n\
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n\
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n\
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n\
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n\
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n\
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n\
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n\
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n\
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n\
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n\
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n\
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n\
rqXRfboQnoZsG4q5WTP468SQvvG5\n\
-----END CERTIFICATE-----"

#define CLIENT_CERT "-----BEGIN CERTIFICATE-----\n\
MIIDWTCCAkGgAwIBAgIUSHy3602J7EsGD9yu7ixD2NhEW8MwDQYJKoZIhvcNAQEL\n\
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n\
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIzMDIwOTA3MzYy\n\
MFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n\
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALrRgSaSI2jO6q910tjq\n\
0IV7mxPyTvToq/+nuYIbwvwZaSepFZ8CIXtCdVYRlvNca/HHbDaPppNsjr0KVapQ\n\
9d2Ms2P/TJjOlAYAZREKpQCXA9tMhgoN/g6/mO5Mr8B0gZBKOCHyFyvFyD7/mrWA\n\
MK5EyW6i6UtuWuTCRDSKX0GUX0xn2Iv0gcskfj7ySJGUWOF1kpMLs2aCXqvUe+nq\n\
UjEJdOAkmqdm6vvqtXBBxlNb6tTJsbV2/sU91sNEa1ixT03I+hyFgCCxckFxN+rd\n\
moAGV1MWOqcrVVgDxKhz7H32DPd36gcDP13u2yPVAy6aEa+hgJ70B9bftX2T7Mp9\n\
HOcCAwEAAaNgMF4wHwYDVR0jBBgwFoAU4nTrfHLAGibVBzv/Ds7MfrRX28MwHQYD\n\
VR0OBBYEFPPvFoIP7tRU4OyEjO+hjmBAkyCNMAwGA1UdEwEB/wQCMAAwDgYDVR0P\n\
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQAx17Im/NYiQOrv6VHxL+dogRij\n\
VldPYZZTxrQ6eHLSiY0aXY70+4Hw0gbNzg1yQJhBXvlwYlCVxl+t49y10I7uxuQ8\n\
vTTQjidIHTVfTQJOS/6H1V6TArCsGQaKnKj2r2OmyGXsbO1CPCj68WzHMSlf9FSm\n\
3k4Blwgu/qYERDCQuU1SW9Lr1yvRTMUo6CXx86tl3mM4Bex3hE0XmrfwwnRc8ed5\n\
8Ko2DjpYszkoqCW+9NzOsd7EqIsvn5FuxyEHk6FpGTibKNkEgnKvesj1eP4qGyjX\n\
A6hTBIDWjDm1vvjKDltcaSKauT5T3U6IDz8r8zhUEWNpNMZywM+K4j75rWXT\n\
-----END CERTIFICATE-----\n\
"

#define CLIENT_KEY "-----BEGIN RSA PRIVATE KEY-----\n\
MIIEowIBAAKCAQEAutGBJpIjaM7qr3XS2OrQhXubE/JO9Oir/6e5ghvC/BlpJ6kV\n\
nwIhe0J1VhGW81xr8cdsNo+mk2yOvQpVqlD13YyzY/9MmM6UBgBlEQqlAJcD20yG\n\
Cg3+Dr+Y7kyvwHSBkEo4IfIXK8XIPv+atYAwrkTJbqLpS25a5MJENIpfQZRfTGfY\n\
i/SByyR+PvJIkZRY4XWSkwuzZoJeq9R76epSMQl04CSap2bq++q1cEHGU1vq1Mmx\n\
tXb+xT3Ww0RrWLFPTcj6HIWAILFyQXE36t2agAZXUxY6pytVWAPEqHPsffYM93fq\n\
BwM/Xe7bI9UDLpoRr6GAnvQH1t+1fZPsyn0c5wIDAQABAoIBAFqY2w7/c9iR79Hj\n\
S7sSNMlm5g/IRG4Un8S9bAMiv25OTGaFYwzXlbEWy5xPr2qZeWepFFtpUpjP3nDi\n\
oCSWWoV0hFKMLnM0SyiV7G1VQiuFEMO62o3ptzkjxWBbJCgq1+JHe+1KX95EbhXP\n\
ExHN9s2sYhsbOhPwkne5liX8rLE6FL76WI4wAWOd6AKk0is225SbjrPGaBvo9YGo\n\
kAyb1/IQGhD7rHBtsYrPsdzxPhmo+h4H0fIg4q0sEhT9OmBmRZYKa7ftJJ6Wpp1D\n\
MJ6+sbYoCs1TBsB/lJOpvTGOPQjLMswE2Q6NitvmbwN1hoNzSPA0+wksUItVlxUF\n\
ACb1tTECgYEA4Y5gVtSYlNDNvz7JtyeLzn9ldYmF37MCosQ4DepOYf1kHu4iA0pH\n\
7PlsA4/Yp8LUDbnlco0w80oiXtU/AyKMaJXQWc8CC501ePLy/MB5pLYzOnFo5L7F\n\
XT5Nz+IuCe5XVOio1FwI7rytuyjZPlS5mOJAmIYL0P52CMlr66b+pWsCgYEA1Aif\n\
8N6EcfNmtr98vCMPyJZMNVYI/lMz0D8SAD/bDUO09g0oOnEg2FdRoWQlsRNRLEKe\n\
ZSDfXvYNBid5WqJMvaLiK892wZnVmUsQM9YPvBOPmvIHpW3pyVslYWBBKPwJZqle\n\
Y2b5RcbDekbCZwECULmtTh58fZop5I0jRlzQSXUCgYBBPJe6pXSYHihT9woKkkKK\n\
cChdLXBt+e16PDxmMe2Hw+0QURm/T92ruZw3D4S/SMdiTPEDB7kvtLJqMU0vT+/C\n\
VApV31ydFVQg7ku/NMA6FoSMk76d4ltdE6cXyDg+oRPbOAuUmCXxPtsUtcTXlVO0\n\
10ONmb+TK/XhprgzCTK/jwKBgC6KShkDroyYgXYhZ86f34oGu2mFnem894WBIoFd\n\
gsZRkzniv268gBocaHWFVuScOdf+M2g9I+pjFY7BQVjMgQ2q3/v/yLBMBN9l44ON\n\
O9zrajz3XQhJrOQeloDeMM8/Kw0WpfopkxNmwmdneqpDRW/UoZ6hAHBNU16wQArk\n\
um81AoGBAJo9WKENnP5gIETmC6RHAFmAYOkToRK27didL+WokGa1YucHViDkfeUL\n\
hmmpO529/TYxVGtPlhI+fs0wv43iA4FDjEWUz6uBWJ7nLgr/5/Ri/VMkLRs9Wi3r\n\
HbgMESnhMzjy9noRYvptAaue60h6uduOBI1k+XvsTzOr8vZZxUw9\n\
-----END RSA PRIVATE KEY-----\n\
"


#define SUCCESS_COMMAND_SIGN_LENGTH sizeof(SUCCESS_COMMAND_SIGN) / sizeof(SUCCESS_COMMAND_SIGN[0])
#define ERROR_COMMAND_SIGN_LENGTH sizeof(ERROR_COMMAND_SIGN) / sizeof(ERROR_COMMAND_SIGN[0])

#define LOG_CONTENT_SIZE 1700
#define SEND_ATTEMPT_DEFAULT 3
#define COMMAND_TIMEOUT_DEFAULT_MS 2000
#define COMMAND_SIZE 1700
#define MODULE_BUFFER_SIZE 100
#define SEND_COMMAND_DELAY_MS 1000


/* Private function prototypes -----------------------------------------------------------------------------*/
static void delay_ms(u32 count);
void setup(struct BC660K * self);
void loop(struct BC660K * self);
enum StatusType sendCommand(struct BC660K * self, u8 send_attempt, u32 command_timeout);
void clearModuleBuffer(struct BC660K *self);

/* AT Command functions */
enum StatusType checkModule_AT(struct BC660K *self);
enum StatusType offEcho_ATE0(struct BC660K *self);
enum StatusType getIMEI_AT_CGSN(struct BC660K *self);
enum StatusType getModelID_AT_CGMM(struct BC660K *self);
enum StatusType checkNetworkRegister_AT_CEREG(struct BC660K *self);
enum StatusType getNetworkStatus_AT_QENG(struct BC660K *self);
enum StatusType setAuthentication_AT_QSSLCFG(struct BC660K *self);
enum StatusType setCACert_AT_QSSLCFG(struct BC660K *self);
enum StatusType setClientCert_AT_QSSLCFG(struct BC660K *self);
enum StatusType setClientPrivateKey_AT_QSSLCFG(struct BC660K *self);
enum StatusType enableSSL_AT_QMTCFG(struct BC660K *self);
enum StatusType openMQTT_AT_QMTOPEN(struct BC660K *self);
enum StatusType connectClient_AT_QMTCONN(struct BC660K *self);
enum StatusType publishMessage_AT_QMTPUB(struct BC660K *self);
enum StatusType disconnectMQTT_AT_QMTDISC(struct BC660K *self);


/* ==================== */

/* Debug */
void writeLog(struct BC660K * self);
char *getStatusTypeString(enum StatusType status);

/* UART ports */
void UART0_GNSS_Configuration(void);
void UART0_Receive(void);

void USART0_MODULE_Configuration(void);
void USART0_Send_Char(u16 Data);
void USART0_Send(char * input_string);
enum StatusType USART0_Receive(struct BC660K *self);

void USART1_DEBUG_Configuration(void);
void USART1_Send_Char(u16 Data);
void USART1_Send(char * input_string);
void USART1_Receive(void);

void LED_Init(void);
void Toggle_LED_1(void);
void Toggle_LED_2(void);
void Toggle_LED_3(void);

/* Private macro -------------------------------------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------------------------------------*/
struct BC660K BC660K_h_h;
vu32 utick;

/* Global functions ----------------------------------------------------------------------------------------*/

/********************************************************************************************************/
/*
 * @brief  Main program.
 * @retval None
 ***********************************************************************************************************/
int main(void) {
  setup(&BC660K_h_h);

  while (1) {
    loop(&BC660K_h_h);
  }
}

/********************************************************************************************************/
/*
 * @brief  Main program.
 * @retval None
 ***********************************************************************************************************/
void setup(struct BC660K * self) {
  /* Initialize system tick */
  SysTick_Config(SystemCoreClock / 1000);
	
	/* Initialize LED */
	LED_Init();

  /* Initialize UART ports */
  UART0_GNSS_Configuration();
  USART0_MODULE_Configuration();
  USART1_DEBUG_Configuration();

  /* Initialize BC660K_handler */
  self->log_content = (char * ) malloc(LOG_CONTENT_SIZE * sizeof(char));
  if (!self -> log_content) {
    Toggle_LED_1();
    while (1);
  }
	
  self->command = (char * ) malloc(COMMAND_SIZE * sizeof(char));
  if (!self -> command) {
    Toggle_LED_1();
    while (1);
  }
	
  self->module_buffer = (char * ) malloc(MODULE_BUFFER_SIZE * sizeof(char));
  if (!self -> module_buffer) {
    Toggle_LED_1();
    while (1);
  }

  sprintf(self -> log_content, "Setup successfully!\n");
  writeLog(self);
}

void loop(struct BC660K * self) {
		checkModule_AT(self);
		offEcho_ATE0(self);
		getIMEI_AT_CGSN(self);
		setAuthentication_AT_QSSLCFG(self);
		setCACert_AT_QSSLCFG(self);
		setClientCert_AT_QSSLCFG(self);
		setClientPrivateKey_AT_QSSLCFG(self);
//		getModelID_AT_CGMM(self);
//		checkNetworkRegister_AT_CEREG(self);
//		getNetworkStatus_AT_QENG(self);
//		disconnectMQTT_AT_QMTDISC(self);
//		openMQTT_AT_QMTOPEN(self);
//		connectClient_AT_QMTCONN(self);
//		publishMessage_AT_QMTPUB(self);
//		publishMessage_AT_QMTPUB(self);
//		disconnectMQTT_AT_QMTDISC(self);
}
	
enum StatusType sendCommand(struct BC660K * self, u8 send_attempt, u32 command_timeout) {
		enum StatusType output_status = STATUS_UNKNOWN;
		if (send_attempt <= 0) {
				send_attempt = SEND_ATTEMPT_DEFAULT;
		}
		u8 count = send_attempt;
		
//		char *command;
//		command = (char * ) malloc(COMMAND_SIZE * sizeof(char));
//		if (!command) {
//			Toggle_LED_1();
//			while (1);
//		}
		
//		strcpy(command, self->command);
		
		while (count--){
				
				sprintf(self->log_content, "\n=== SENDING <%s> | ATTEMPT %u/%u ===\n", self->command, (send_attempt-count), send_attempt);
				writeLog(self);
			
				clearModuleBuffer(self);
				
				
				USART0_Send(self->command);
				USART0_Send((char *)"\r\n");

				self->command_timer = utick;
				while(utick - self->command_timer <= COMMAND_TIMEOUT_DEFAULT_MS) {
						output_status = USART0_Receive(self);
				}
				
				sprintf(self->log_content, "%s\n\n", self->module_buffer);
				writeLog(self);
				clearModuleBuffer(self);
				sprintf(self->log_content, "Command status: %s\n", getStatusTypeString(output_status));
				writeLog(self);
				sprintf(self->log_content, "==========\n");
				writeLog(self);
				
				delay_ms(SEND_COMMAND_DELAY_MS);
				
				if (output_status == STATUS_SUCCESS) {
						break;
				}
		}
		
		return output_status;
}

void clearModuleBuffer(struct BC660K *self) {
		for (self->module_buffer_index = 0; self->module_buffer_index < MODULE_BUFFER_SIZE; self->module_buffer_index++) {
				self->module_buffer[self->module_buffer_index] = 0;
		}
		self->module_buffer_index = 0;
}

enum StatusType checkModule_AT(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT");
		output_status = sendCommand(self, SEND_ATTEMPT_DEFAULT, COMMAND_TIMEOUT_DEFAULT_MS);
	
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}
		
		return output_status;
}

enum StatusType offEcho_ATE0(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "ATE0");
		output_status = sendCommand(self, SEND_ATTEMPT_DEFAULT, COMMAND_TIMEOUT_DEFAULT_MS);
	
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}
		
		return output_status;
}

enum StatusType getIMEI_AT_CGSN(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT+CGSN");
		output_status = sendCommand(self, SEND_ATTEMPT_DEFAULT, COMMAND_TIMEOUT_DEFAULT_MS);
	
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}
		
		return output_status;
}

enum StatusType getModelID_AT_CGMM(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT+CGMM");
		output_status = sendCommand(self, SEND_ATTEMPT_DEFAULT, COMMAND_TIMEOUT_DEFAULT_MS);
	
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}
		
		return output_status;
}

enum StatusType checkNetworkRegister_AT_CEREG(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT+CEREG?");
		output_status = sendCommand(self, SEND_ATTEMPT_DEFAULT, COMMAND_TIMEOUT_DEFAULT_MS);
	
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}
		
		return output_status;
}

enum StatusType getNetworkStatus_AT_QENG(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT+QENG=0");
		output_status = sendCommand(self, SEND_ATTEMPT_DEFAULT, COMMAND_TIMEOUT_DEFAULT_MS);
	
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}
		
		return output_status;
}

enum StatusType openMQTT_AT_QMTOPEN(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT+QMTOPEN=0,\"broker.hivemq.com\",1883");
		output_status = sendCommand(self, SEND_ATTEMPT_DEFAULT, COMMAND_TIMEOUT_DEFAULT_MS);
	
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}
		
		return output_status;
}
enum StatusType connectClient_AT_QMTCONN(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT+QMTCONN=0,\"anhttm8client\"");
		output_status = sendCommand(self, SEND_ATTEMPT_DEFAULT, COMMAND_TIMEOUT_DEFAULT_MS);
	
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}
		
		return output_status;
}

enum StatusType publishMessage_AT_QMTPUB(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		
		/* Write Command */
		sprintf(self->command, "AT+QMTPUB=0,0,0,0,\"topic/pub\",5");
		sprintf(self->log_content, "\n=== SENDING <%s> ===\n", self->command);
		writeLog(self);
		clearModuleBuffer(self);
	
		USART0_Send(self->command);
		USART0_Send((char *)"\r\n");

		self->command_timer = utick;
		while(utick - self->command_timer <= COMMAND_TIMEOUT_DEFAULT_MS) {
				output_status = USART0_Receive(self);
		}
		
		sprintf(self->log_content, "%s", self->module_buffer);
		writeLog(self);
		clearModuleBuffer(self);
		
		sprintf(self->command, "hello");
		USART0_Send(self->command);
		USART0_Send((char *)"\r\n");
	
		self->command_timer = utick;
		while(utick - self->command_timer <= (COMMAND_TIMEOUT_DEFAULT_MS + 2000)) {
				output_status = USART0_Receive(self);
		}
		
		sprintf(self->log_content, "%s\n\n", self->module_buffer);
		writeLog(self);
		clearModuleBuffer(self);
		sprintf(self->log_content, "Command status: %s\n", getStatusTypeString(output_status));
		writeLog(self);
		sprintf(self->log_content, "==========\n");
		writeLog(self);
		
		delay_ms(SEND_COMMAND_DELAY_MS);
		
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}
		
		return output_status;
}

enum StatusType disconnectMQTT_AT_QMTDISC(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT+QMTDISC=0");
		output_status = sendCommand(self, 2, COMMAND_TIMEOUT_DEFAULT_MS);
	
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}
		
		return output_status;
}

enum StatusType setAuthentication_AT_QSSLCFG(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT+QSSLCFG=0,0,\"seclevel\",2");
		output_status = sendCommand(self, SEND_ATTEMPT_DEFAULT, COMMAND_TIMEOUT_DEFAULT_MS);
	
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}
		
		return output_status;
}

enum StatusType setCACert_AT_QSSLCFG(struct BC660K *self)  {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT+QSSLCFG=0,0,\"cacert\"");
		sprintf(self->log_content, "\n=== SENDING <%s> ===\n", self->command);
		writeLog(self);
		clearModuleBuffer(self);
	
		USART0_Send(self->command);
		USART0_Send((char *)"\r\n");

		self->command_timer = utick;
		while(utick - self->command_timer <= COMMAND_TIMEOUT_DEFAULT_MS) {
				output_status = USART0_Receive(self);
		}
		
		sprintf(self->log_content, "%s", self->module_buffer);
		writeLog(self);
		clearModuleBuffer(self);
		
		sprintf(self->command, CA_CERT);
		USART0_Send(self->command);
		USART0_Send((char *)"\r\n");
		delay_ms(100);
		USART0_Send_Char(26);
	
		self->command_timer = utick;
		while(utick - self->command_timer <= (COMMAND_TIMEOUT_DEFAULT_MS + 2000)) {
				output_status = USART0_Receive(self);
		}
		
		sprintf(self->log_content, "%s\n\n", self->module_buffer);
		writeLog(self);
		clearModuleBuffer(self);
		sprintf(self->log_content, "Command status: %s\n", getStatusTypeString(output_status));
		writeLog(self);
		sprintf(self->log_content, "==========\n");
		writeLog(self);
		
		delay_ms(SEND_COMMAND_DELAY_MS);
		
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}
		
		return output_status;
}

enum StatusType setClientCert_AT_QSSLCFG(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT+QSSLCFG=0,0,\"clientcert\"");
		sprintf(self->log_content, "\n=== SENDING <%s> ===\n", self->command);
		writeLog(self);
		clearModuleBuffer(self);
	
		USART0_Send(self->command);
		USART0_Send((char *)"\r\n");

		self->command_timer = utick;
		while(utick - self->command_timer <= COMMAND_TIMEOUT_DEFAULT_MS) {
				output_status = USART0_Receive(self);
		}
		
		sprintf(self->log_content, "%s", self->module_buffer);
		writeLog(self);
		clearModuleBuffer(self);
		
		sprintf(self->command, CLIENT_CERT);
		USART0_Send(self->command);
		USART0_Send((char *)"\r\n");
		delay_ms(100);
		USART0_Send_Char(26);
	
		self->command_timer = utick;
		while(utick - self->command_timer <= (COMMAND_TIMEOUT_DEFAULT_MS + 2000)) {
				output_status = USART0_Receive(self);
		}
		
		sprintf(self->log_content, "%s\n\n", self->module_buffer);
		writeLog(self);
		clearModuleBuffer(self);
		sprintf(self->log_content, "Command status: %s\n", getStatusTypeString(output_status));
		writeLog(self);
		sprintf(self->log_content, "==========\n");
		writeLog(self);
		
		delay_ms(SEND_COMMAND_DELAY_MS);
		
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}
		
		return output_status;
}

enum StatusType setClientPrivateKey_AT_QSSLCFG(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT+QSSLCFG=0,0,\"clientkey\"");
		sprintf(self->log_content, "\n=== SENDING <%s> ===\n", self->command);
		writeLog(self);
		clearModuleBuffer(self);
	
		USART0_Send(self->command);
		USART0_Send((char *)"\r\n");

		self->command_timer = utick;
		while(utick - self->command_timer <= COMMAND_TIMEOUT_DEFAULT_MS) {
				output_status = USART0_Receive(self);
		}
		
		sprintf(self->log_content, "%s", self->module_buffer);
		writeLog(self);
		clearModuleBuffer(self);
		
		sprintf(self->command, CLIENT_KEY);
		USART0_Send(self->command);
		USART0_Send((char *)"\r\n");
		delay_ms(100);
		USART0_Send_Char(26);
	
		self->command_timer = utick;
		while(utick - self->command_timer <= (COMMAND_TIMEOUT_DEFAULT_MS + 2000)) {
				output_status = USART0_Receive(self);
		}
		
		sprintf(self->log_content, "%s\n\n", self->module_buffer);
		writeLog(self);
		clearModuleBuffer(self);
		sprintf(self->log_content, "Command status: %s\n", getStatusTypeString(output_status));
		writeLog(self);
		sprintf(self->log_content, "==========\n");
		writeLog(self);
		
		delay_ms(SEND_COMMAND_DELAY_MS);
		
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}
		
		return output_status;
}

enum StatusType enableSSL_AT_QMTCFG(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT+QMTCFG=\"ssl\",0,1,0,0");
		output_status = sendCommand(self, SEND_ATTEMPT_DEFAULT, COMMAND_TIMEOUT_DEFAULT_MS);
	
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}
		
		return output_status;
}


/* Debug */
void writeLog(struct BC660K * self) {
  USART1_Send(self -> log_content);
}

char *getStatusTypeString(enum StatusType status) {
		switch(status) {
			case STATUS_SUCCESS:
					return "SUCCESS";
			case STATUS_ERROR:
					return "ERROR";
			case STATUS_TIMEOUT:
					return "TIMEOUT";
			case STATUS_BAD_PARAMETERS:
					return "PARAMETERS";
			case STATUS_UNKNOWN:
					return "UNKNOWN";
			default:
					return "UNSUPPORTED STATUS";
		}
}

/*************************************************************************************************************
 * @brief  Configure the UART0 for GNSS
 * @retval None
 ***********************************************************************************************************/
void UART0_GNSS_Configuration(void) {
  CKCU_PeripClockConfig_TypeDef CKCUClock; // Set all the fields to zero, which means that no peripheral clocks are enabled by default.

  {
    /* Enable peripheral clock of AFIO, UxART                                                                 */
    CKCUClock.Bit.AFIO = 1;
    CKCUClock.Bit.PB = 1;
    CKCUClock.Bit.UART0 = 1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);
  }

  /* Turn on UxART Rx internal pull up resistor to prevent unknow state                                     */
  GPIO_PullResistorConfig(HT_GPIOB, GPIO_PIN_8, GPIO_PR_UP);

  /* Config AFIO mode as UxART function.                                                                    */
  AFIO_GPxConfig(GPIO_PB, AFIO_PIN_7, AFIO_FUN_USART_UART);
  AFIO_GPxConfig(GPIO_PB, AFIO_PIN_8, AFIO_FUN_USART_UART);

  {
    /* UxART configured as follow:
          - BaudRate = 115200 baud
          - Word Length = 8 Bits
          - One Stop Bit
          - None parity bit
    */

    /* !!! NOTICE !!!
       Notice that the local variable (structure) did not have an initial value.
       Please confirm that there are no missing members in the parameter settings below in this function.
    */
    USART_InitTypeDef USART_InitStructure = {
      0
    };
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WORDLENGTH_8B;
    USART_InitStructure.USART_StopBits = USART_STOPBITS_1;
    USART_InitStructure.USART_Parity = USART_PARITY_NO;
    USART_InitStructure.USART_Mode = USART_MODE_NORMAL;
    USART_Init(HT_UART0, & USART_InitStructure);
  }

  /* Enable UxART Tx and Rx function                                                                        */
  USART_TxCmd(HT_UART0, ENABLE);
  USART_RxCmd(HT_UART0, ENABLE);
}

/*************************************************************************************************************
 * @brief  Configure the USART0
 * @retval None
 ***********************************************************************************************************/
void USART0_MODULE_Configuration(void) {
  CKCU_PeripClockConfig_TypeDef CKCUClock; // Set all the fields to zero, which means that no peripheral clocks are enabled by default.

  {
    /* Enable peripheral clock of AFIO, UxART                                                                 */
    CKCUClock.Bit.AFIO = 1;
    CKCUClock.Bit.PA = 1;
    CKCUClock.Bit.USART0 = 1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);
  }

  /* Turn on UxART Rx internal pull up resistor to prevent unknow state                                     */
  GPIO_PullResistorConfig(HT_GPIOA, GPIO_PIN_3, GPIO_PR_UP);

  /* Config AFIO mode as UxART function.                                                                    */
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_2, AFIO_FUN_USART_UART);
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_3, AFIO_FUN_USART_UART);

  {
    /* UxART configured as follow:
          - BaudRate = 115200 baud
          - Word Length = 8 Bits
          - One Stop Bit
          - None parity bit
    */

    /* !!! NOTICE !!!
       Notice that the local variable (structure) did not have an initial value.
       Please confirm that there are no missing members in the parameter settings below in this function.
    */
    USART_InitTypeDef USART_InitStructure = {
      0
    };
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WORDLENGTH_8B;
    USART_InitStructure.USART_StopBits = USART_STOPBITS_1;
    USART_InitStructure.USART_Parity = USART_PARITY_NO;
    USART_InitStructure.USART_Mode = USART_MODE_NORMAL;
    USART_Init(HT_USART0, & USART_InitStructure);
  }

  /* Enable UxART Tx and Rx function                                                                        */
  USART_TxCmd(HT_USART0, ENABLE);
  USART_RxCmd(HT_USART0, ENABLE);
}

/********************************************************************************************************/
/*
 * @brief  Configure the USART1
 * @retval None
 ***********************************************************************************************************/
void USART1_DEBUG_Configuration(void) {
  CKCU_PeripClockConfig_TypeDef CKCUClock; // Set all the fields to zero, which means that no peripheral clocks are enabled by default.

  {
    /* Enable peripheral clock of AFIO, UxART                                                                 */
    CKCUClock.Bit.AFIO = 1;
    CKCUClock.Bit.PA = 1;
    CKCUClock.Bit.USART1 = 1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);
  }

  /* Turn on UxART Rx internal pull up resistor to prevent unknow state                                     */
  GPIO_PullResistorConfig(HT_GPIOA, GPIO_PIN_5, GPIO_PR_UP);

  /* Config AFIO mode as UxART function.                                                                    */
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_4, AFIO_FUN_USART_UART);
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_5, AFIO_FUN_USART_UART);

  {
    /* UxART configured as follow:
          - BaudRate = 115200 baud
          - Word Length = 8 Bits
          - One Stop Bit
          - None parity bit
    */

    /* !!! NOTICE !!!
       Notice that the local variable (structure) did not have an initial value.
       Please confirm that there are no missing members in the parameter settings below in this function.
    */
    USART_InitTypeDef USART_InitStructure = {
      0
    };
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WORDLENGTH_8B;
    USART_InitStructure.USART_StopBits = USART_STOPBITS_1;
    USART_InitStructure.USART_Parity = USART_PARITY_NO;
    USART_InitStructure.USART_Mode = USART_MODE_NORMAL;
    USART_Init(HT_USART1, & USART_InitStructure);
  }

  /* Enable UxART Tx and Rx function                                                                        */
  USART_TxCmd(HT_USART1, ENABLE);
  USART_RxCmd(HT_USART1, ENABLE);
}

/********************************************************************************************************/
/*
 * @brief  UxART0 Tx Send Byte.
 * @param  Data: the data to be transmitted.
 * @retval None
 ***********************************************************************************************************/
void USART0_Send_Char(u16 Data) {
  while (USART_GetFlagStatus(HT_USART0, USART_FLAG_TXC) == RESET);
  USART_SendData(HT_USART0, Data);
}

/********************************************************************************************************/
/*
 * @brief  UxART Tx Send Byte.
 * @param  Data: the data to be transmitted.
 * @retval None
 ***********************************************************************************************************/
void USART1_Send_Char(u16 Data) {
  while (USART_GetFlagStatus(HT_USART1, USART_FLAG_TXC) == RESET);
  USART_SendData(HT_USART1, Data);
}

/********************************************************************************************************/
/*
 * @brief  UxART Tx Test.
 * @retval None
 ***********************************************************************************************************/
void USART0_Send(char * input_string) {
  int i;
  /* Send a buffer from UxART to terminal                                                                   */
  for (i = 0; i < strlen(input_string); i++) {
    USART0_Send_Char(input_string[i]);
  }

  /* Send to USART1 what sent to USART0 */
  USART1_Send(input_string);
}

/********************************************************************************************************/
/*
 * @brief  UxART Tx Test.
 * @retval None
 ***********************************************************************************************************/
void USART1_Send(char * input_string) {
  int i;
  /* Send a buffer from UxART to terminal                                                                   */
  for (i = 0; i < strlen(input_string); i++) {
    USART1_Send_Char(input_string[i]);
  }
}

void UART0_Receive(void) {
  u16 uData;

  /* Waits until the Rx FIFO/DR is not empty then get data from them                                        */
  if (USART_GetFlagStatus(HT_UART0, USART_FLAG_RXDR) == SET) {
    uData = USART_ReceiveData(HT_UART0);

    #if 1 // Loop back Rx data to Tx for test
    USART1_Send_Char(uData);
    #endif
  }
}

enum StatusType USART0_Receive(struct BC660K *self) {
		enum StatusType output_status = STATUS_TIMEOUT;
		u16 uData;
		u8 index;
		char *ptr;

		/* Waits until the Rx FIFO/DR is not empty then get data from them                                        */
		if (USART_GetFlagStatus(HT_USART0, USART_FLAG_RXDR) == SET) {
			uData = USART_ReceiveData(HT_USART0);
			self->module_buffer[self->module_buffer_index] = uData;
			self->module_buffer_index++;
		}

		for (index = 0; index < SUCCESS_COMMAND_SIGN_LENGTH; index++){
				ptr = strstr(self->module_buffer, SUCCESS_COMMAND_SIGN[index]);
				if (ptr) {
						output_status = STATUS_SUCCESS;
						return output_status;
				}
		}
		
		for (index = 0; index < ERROR_COMMAND_SIGN_LENGTH; index++){
				ptr = strstr(self->module_buffer, ERROR_COMMAND_SIGN[index]);
				if (ptr) {
						output_status = STATUS_ERROR;
						return output_status;
				}
		}
		
		return output_status;
}

void USART1_Receive(void) {
  u16 uData;

  /* Waits until the Rx FIFO/DR is not empty then get data from them                                        */
  if (USART_GetFlagStatus(HT_USART1, USART_FLAG_RXDR) == SET) {
    uData = USART_ReceiveData(HT_USART1);

    #if 1 // Loop back Rx data to Tx for test
    USART1_Send_Char(uData);
    #endif
  }
}

void LED_Init() {
  HT32F_DVB_LEDInit(HT_LED1);
  HT32F_DVB_LEDInit(HT_LED2);
  HT32F_DVB_LEDInit(HT_LED3);
  HT32F_DVB_LEDOn(HT_LED1);
  HT32F_DVB_LEDOff(HT_LED2);
  HT32F_DVB_LEDOn(HT_LED3);
}

void Toggle_LED_1() {
    delay_ms(100);
    HT32F_DVB_LEDToggle(HT_LED1);
}

void Toggle_LED_2() {
    delay_ms(100);
    HT32F_DVB_LEDToggle(HT_LED2);
}

void Toggle_LED_3() {
    delay_ms(100);
    HT32F_DVB_LEDToggle(HT_LED3);
}

#if(HT32_LIB_DEBUG == 1)
/********************************************************************************************************/
/*
 * @brief  Report both the error name of the source file and the source line number.
 * @param  filename: pointer to the source file name.
 * @param  uline: error line source number.
 * @retval None
 ***********************************************************************************************************/
void assert_error(u8 * filename, u32 uline) {
  /*
     This function is called by IP library that the invalid parameters has been passed to the library API.
     Debug message can be added here.
     Example: printf("Parameter Error: file %s on line %d\r\n", filename, uline);
  */

  while (1) {}
}
#endif

/* Private functions ---------------------------------------------------------------------------------------*/
/********************************************************************************************************/
/*
 * @brief  delay function
 * @param  count: delay count for loop
 * @retval None
 ***********************************************************************************************************/
static void delay_ms(u32 ms) {
  uint32_t i, j;
  for (i = 0; i < ms; i++) {
    for (j = 0; j < 33132; j++) {
      __NOP();
    }
  }
}

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */
