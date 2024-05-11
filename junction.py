import eel

#Pour que eel fonction changer la blibliothèque :
#https://stackoverflow.com/questions/77232001/python-eel-module-unable-to-use-import-bottle-ext-websocket-as-wbs-modulenotfoun
#car python 3.12 ne le prend plus en charge

led_val = "0"

eel.init("site_web")

@eel.expose
def readfile(filename):
    file = open(filename,"r")
    lines = file.readlines()
    lum = lines[0].split(":")[1]
    angleX = int(lines[1].split(":")[1])
    angleY = int(lines[2].split(":")[1])
    angleZ = int(lines[3].split(":")[1])
    bouton = int(lines[4].split(":")[1])
    data = [lum,angleX,angleY,angleZ,bouton]
    #print(data)
    return data

@eel.expose
def writefile(data,type,filename):
#data : valeur qu'on écrire
#type : sur quoi ? (led, écran oled, ...)

    file = open(filename, "w")
    if(type == "led"):
        led_val = data
    file.write("led: " + led_val)

eel.start("index.html", mode='default')
