from paho.mqtt import client as mqtt

MQTT_BROKER = "localhost"
MQTT_TOPIC_LUM = "lum"
MQTT_TOPIC_BTN = "btn"
MQTT_TOPIC_AX = "angle_x"
MQTT_TOPIC_AY = "angle_y"

DATA_BASE = "../database2web.txt"

lum_val = 0
angleX = 0
angleY = 0
angleZ = 0
btn_val = 0

data = [lum_val,angleX,angleY,angleZ, btn_val]

def writefile(data,filename):
#data : valeur qu'on écrire

  file = open(filename, "w")
  
  file.write("lum: ") 
  file.write(str(data[0]))
  file.write("%") 
  file.write("\n")
  
  file.write("angleX: ") 
  file.write(str(data[1]))
  file.write("\n")
  
  file.write("angleY: ") 
  file.write(str(data[2]))
  file.write("\n")
  
  file.write("angleZ: ") 
  file.write(str(data[3]))
  file.write("\n")
  
  file.write("btn: ") 
  file.write(str(data[4]))
  file.write("\n")
  
  file.close()


def on_connect( client, userdata, flags, rc):
  if (rc == 0):
    client.subscribe(MQTT_TOPIC_LUM)
    print("subscribe to :\"", MQTT_TOPIC_LUM, "\"")
    client.subscribe(MQTT_TOPIC_BTN)
    print("subscribe to :\"", MQTT_TOPIC_BTN, "\"")
    client.subscribe(MQTT_TOPIC_AX)
    print("subscribe to :\"", MQTT_TOPIC_AX, "\"")
    client.subscribe(MQTT_TOPIC_AY)
    print("subscribe to :\"", MQTT_TOPIC_AY, "\"")
  else : 
    print("bad connection to :\"", MQTT_TOPIC_LUM, "\"")

def on_message(client, userdata, msg):
  
  topic_name = str(msg.topic) 
  if(topic_name == "lum"):
    print("lum_val = ", int(msg.payload.decode("utf-8")))
    global lum_val
    lum_val = int(msg.payload.decode("utf-8"))
  
  if(topic_name == "btn"):
    print("btn_val = ", int(msg.payload.decode("utf-8")))
    global btn_val
    btn_val = int(msg.payload.decode("utf-8"))
    
  if(topic_name == "angle_x"):
    print("angle_x = ", int(msg.payload.decode("utf-8")))
    global angleX
    angleX = int(msg.payload.decode("utf-8"))
  
  if(topic_name == "angle_y"):
    print("angle_y = ", int(msg.payload.decode("utf-8")))
    global angleY
    angleY = int(msg.payload.decode("utf-8"))
  
  data = [lum_val,angleX,angleY,angleZ,btn_val]
  
  writefile(data,DATA_BASE)
  

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect(MQTT_BROKER)
client.loop_forever()
