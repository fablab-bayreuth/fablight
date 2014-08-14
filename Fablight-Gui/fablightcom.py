
import time
import serial
from serial.tools.list_ports import comports

def list_ports():
    ports = [p[0] for p in comports()]
    return ports


class FablightCom(object):
    _port = None
    portname = None

    def __init__(self, portname=None, baudrate=None):
        if portname!= None:
            self.open(portname, baudrate)

    def open(self, portname, baudrate=None):
        if baudrate==None: baudrate=38400
        self._port = serial.Serial(portname, baudrate=baudrate,
                                   timeout=1)
        # Note: By default, the Arduino resets when opening the Com port, needs
        #   1-2 seconds to boot. The firmware sends a greeting when up and running.
        self._port.timeout = 0.05
        if not self.is_fablight():
            # May be busy booting, wait for greeting message
            self._port.timeout = 3
            self.readline()
        self._port.flushInput()
        if not self.is_fablight():
            # If still not responding, something is wrong
            self._port.close()
            print "Error: Did not recognize a Fablight at port %s" % portname
        self.portname = portname

    def is_open(self):
        return self._port != None

    def close(self):
        self._port.close()
        self._port = None
        self.portname = None

    def __del__(self):
        try: self.close()
        except: pass

    def write(self, data):
        if self._port==None:
            print 'Error: serial port not opened.'
            return
        if not (data[-1] in ['\r', '\n'] ):
            data += '\r'
        self._port.write(data)

    def readline(self):
        if self._port==None:
            print 'Error: serial port not opened.'
            return
        return self._port.readline().strip()

    def ask(self, cmd):
        self.write(cmd)
        return self.readline()

    def off(self):
        self.write('off')

    def set_rgbw( self, red=0, green=0, blue=0, white=0):
        clip = lambda x,lo,hi: lo if x<lo else (hi if x>hi else x)
        red = clip(red,0,255)
        green = clip(green, 0, 255)
        blue = clip(blue, 0, 255)
        white = clip(white, 0, 255)
        cmd = 'RGBW:%02x%02x%02x%02x' % (red,green,blue,white)
        self.write(cmd)

    def get_id( self ):
        self._port.flushInput()
        return self.ask('id?')

    def is_fablight( self ):
        self._port.flushInput()
        a = self.ask('fablight?')
        return a.startswith('YESSIR!')
    
