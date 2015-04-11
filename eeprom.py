import serial

ser = ''

wrf=[]

page=0

def openser():
    global ser
    ser = serial.Serial('/dev/ttyUSB0',57600 , timeout=2)


def trfbuff():
    for x in xrange(128):
        ser.write(str(wrf[x])+"\n")
        print ser.readline()
    print ser.readline()
        

def pr():
    w = 1
    strout = ""
    while (1):
        tmp = ser.read(1)
        if tmp == "\x03":
            break
        strout += tmp
        
    return strout

def write(strcmd):
    ser.write(strcmd+"\n")
    return pr()
    

def close():
    ser.close()

def decode(rpl):
    return rpl.split("\n")[1].split(";")


def openfile():
    #filep= open("/home/note/Downloads/beeride/beer 202/firmware_1.9/hddrom19.rom","rb")
    filep= open("/home/note/Downloads/TECHNICL","rb")
    for bt in filep.read(): 
      wrf.append(bt)
    filep.close()

def loadPageRom(p):
    write("p"+str(p))
    return decode(write("r"))

def vrfall():
    p=0
    i=0
    t = loadPageRom(p)
    for x in wrf:
        if ord(x) != int(t[i],16):
            print "erro"
            break

        i +=1
        
        if i==128:
            i=0
            p+=1
            t = loadPageRom(p)
            print "  p: ",p,


openser()
