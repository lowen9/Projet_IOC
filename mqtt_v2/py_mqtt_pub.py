from paho.mqtt import client as mqtt
import time

MQTT_BROKER = "localhost"
MQTT_TOPIC = "led"

DATA_BASE = "../database2esp.txt"

message_prev = " OFF"

flag_connected = 0

def readfile(filename):
    file = open(filename,"r")
    lines = file.readlines()
    led = lines[0].split(":")[1]
    data = [led]
    #print(data)
    return data

def on_connect( client, userdata, flags, rc):
	flag_connected = 1
	if( rc == 0 ):
		print("publish to :\"", MQTT_TOPIC, "\"")
	else:
		print("bad connection to :\"", MQTT_TOPIC, "\"")

def on_disconnect( client, userdata, rc):
	flag_connected = 0
	print("disconnect to :\"", MQTT_TOPIC, "\"") 
        
def on_publish(client, userdata, mid):
    print("message : sent! check the led")

client = mqtt.Client()
client.on_connect = on_connect
client.on_disconnect = on_disconnect
client.on_publish = on_publish

client.connect(MQTT_BROKER)

while True :
	message = readfile(DATA_BASE)
	if(message[0] != message_prev):
		message_prev = message[0]	
		print(message[0])
		if(flag_connected == 0):
			client.connect(MQTT_BROKER)
		client.publish(MQTT_TOPIC, message[0])
	time.sleep(0.5)
	
