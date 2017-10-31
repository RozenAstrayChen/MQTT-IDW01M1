#include "mbed.h"
#include "SpwfInterface.h"
#include "TCPSocket.h"
#include "MQTTClient.h"
#include "MQTTWiFi.h"

// MQTT use
#define MQTT_MAX_PACKET_SIZE 250
#define MQTT_MAX_PAYLOAD_SIZE 300
//Configuration value needed to connect Red-node
#define BROKER_URL "192.168.0.10"
#define MQTT_PORT 1883
//MQTT use Topic 
#define TOPIC "fuck"


//Wifi network
#define SSID "megu megu fire"
#define PASSW "66666667"

DigitalOut myled(LED1);

int connack_rc = 0;    // MQTT connack return code
const char * ip_addr = "";
char *host_addr = "";
bool netConnecting = false;
int connectTimeout = 1000;
bool mqttConnecting = false;
bool netConnected = false;
bool connected = false;
int retryAttempt = 0;
char subscription_url[MQTT_MAX_PAYLOAD_SIZE];

MQTT::Message message;
MQTTString TopicName={TOPIC};
MQTT::MessageData MsgData(TopicName, message);

int connect(MQTT::Client<MQTTWiFi, Countdown, MQTT_MAX_PACKET_SIZE>* client,MQTTWiFi* ipstack){
    char* hostname = "192.168.0.10";
    SpwfSAInterface& WiFi = ipstack->getWiFi();

    //Network Debug statements
    LOG("=====================================\n\r");
    LOG("Connecting WiFi.\n\r");
    LOG("Nucleo IP ADDRESS: %s\n\r", WiFi.get_ip_address());
    LOG("Nucleo MAC ADDRESS: %s\n\r", WiFi.get_mac_address());
    LOG("Server Hostname: %s port: %d\n\r", hostname, MQTT_PORT);
    LOG("Topic: %s\n\r", TOPIC);
    //need subscrie
    LOG("=====================================\n\r");
    netConnecting = true;
    ipstack->open(&ipstack->getWiFi());
    int rc = ipstack->connect(hostname,MQTT_PORT,connectTimeout);
     if (rc != 0)
    {
        WARN("IP Stack connect returned: %d\n", rc);    
        return rc;
    }
    printf ("--->TCP Connected\n\r");
    netConnected = true;
    netConnecting = false;

    //MQTT Connect
    mqttConnecting = true;
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 4;
    data.struct_version =0;

    if((rc = client->connect(data)) == 0){
    	connected = true;
    	printf("--->MQTT Connected\n\r");
    	 /*not to do subscrie*/
    }else {
        WARN("MQTT connect returned %d\n", rc);        
    }
    if (rc >= 0)
        connack_rc = rc;
        mqttConnecting = false;
        return rc;
   
}
int getConnTimeout(int attemptNumber)
{  // First 10 attempts try within 3 seconds, next 10 attempts retry after every 1 minute
   // after 20 attempts, retry every 10 minutes
    return (attemptNumber < 10) ? 3 : (attemptNumber < 20) ? 60 : 600;
}
void attemptConnect(MQTT::Client<MQTTWiFi, Countdown, MQTT_MAX_PACKET_SIZE>* client, MQTTWiFi* ipstack)
{
    connected = false;
           
    while (connect(client, ipstack) != MQTT_CONNECTION_ACCEPTED) 
    {    
        /*if (connack_rc == MQTT_NOT_AUTHORIZED || connack_rc == MQTT_BAD_USERNAME_OR_PASSWORD) {
            printf ("File: %s, Line: %d Error: %d\n\r",__FILE__,__LINE__, connack_rc);        
            return; // don't reattempt to connect if credentials are wrong
        } */
        int timeout = getConnTimeout(++retryAttempt);
        WARN("Retry attempt number %d waiting %d\n", retryAttempt, timeout);
        
        // if ipstack and client were on the heap we could deconstruct and goto a label where they are constructed
        //  or maybe just add the proper members to do this disconnect and call attemptConnect(...)        
        // this works - reset the system when the retry count gets to a threshold
        if (retryAttempt == 5)
            NVIC_SystemReset();
        else
            wait(timeout);
    }
}
int publish (MQTT::Client<MQTTWiFi, Countdown, MQTT_MAX_PACKET_SIZE>* client,MQTTWiFi* ipstack){
	MQTT::Message message;
	char *pubTopic = TOPIC;
	/*send data*/
	char buf[MQTT_MAX_PAYLOAD_SIZE] = {"NUCLEO_F401RE"};
	message.qos = MQTT::QOS0;
	message.retained = false;
    message.dup = false;
    message.payload = (void*)buf;
    message.payloadlen = strlen(buf);

    printf("Publishing %s\n\r", buf);
    return client->publish(pubTopic, message);
}

int main(int argc, char const *argv[])
{
	myled =0;
	/* code */
	const char *ssid = SSID;
	const char *seckey = PASSW;
	//use SpwfSAInterface connect AP
	SpwfSAInterface spwf(D8,D2, false);

	printf("\r\nX-NUCLEO-IDW01M1 mbed \n");
	printf("\r\nconnecting to AP\n");
	//connect to Wifi
	MQTTWiFi ipstack(spwf, ssid, seckey, NSAPI_SECURITY_WPA2);
	//check wifi has got ip_address
	if(ipstack.getWiFi().get_ip_address() == 0){
		printf("Connect WiFi is failed!\nPlease check your ssid and passwd is correct");
		return 0;
	}
	printf("ip: %s\n",ipstack.getWiFi().get_ip_address() );

	MQTT::Client<MQTTWiFi, Countdown, MQTT_MAX_PACKET_SIZE> client(ipstack);
	attemptConnect(&client, &ipstack);

	myled=1;         
   int count = 0;    
//    tyeld.start();    
    while (true)
    {
        if (++count == 100)
        {               // Publish a message every second
            if (publish(&client, &ipstack) != 0) { 
                myled=0;
                attemptConnect(&client, &ipstack);   // if we have lost the connection                
            } else myled=1;
            count = 0;
        }        
//        int start = tyeld.read_ms();
        client.yield(10);  // allow the MQTT client to receive messages
//        printf ("tyeld: %d\n\r",tyeld.read_ms()-start);
    }
	
	return 0;
}