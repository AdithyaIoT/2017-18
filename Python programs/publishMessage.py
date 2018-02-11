import paho.mqtt.client as mqtt
import sys

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
                
# The callback method which is called by the server when a PUBLISH message is received from the server.
def on_publish(client, userdata, mid):
    print("mid =   "+str(mid))
                    

#defining the client ID format for IBM Watson is d:orgid:devicetype:deviceid
client = mqtt.Client(client_id="d:9qib2v:MKR1000:AdiHealthBand",clean_session=True,userdata=None)

#setting the authentication token for IBM Watson
client.username_pw_set("use-token-auth", "Robotman02!!")
client.on_connect = on_connect
client.on_publish = on_publish

#Connecting to the broker 
client.connect("9qib2v.messaging.internetofthings.ibmcloud.com", 1883, 120)
client.loop_start()
param=sys.argv[1]
value=sys.argv[2];

while True: 
	json = "{\"9qib2v\":{\"MKR1000\":\"AdiHealthBand\",\"" + param + "\":\"" + str(value) + "\" } } ";
	print(json)
	(rc, mid) =client.publish("iot-2/evt/status/fmt/json", json, qos=1)
	if (rc == 0) :
		break
		
client.loop_stop()
