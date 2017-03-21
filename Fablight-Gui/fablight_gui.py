import sys, threading
from numpy import *
from colorsys import *

import Tkinter as tk
import ttk
import PIL.Image, PIL.ImageTk

import fablightcom
fabcom = fablightcom.FablightCom()

import rgb_picker
reload(rgb_picker)
from rgb_picker import RGB_Picker

import hsv_picker
reload(hsv_picker)
from hsv_picker import HSV_Picker, H_SV_Picker, HS_V_Picker

import hcl_picker
reload(hcl_picker)
from hcl_picker import H_CL_Picker

import colorbc
reload(colorbc)
from colorbc import Color_Broadcast

import colordisplay
reload(colordisplay)
from colordisplay import Color_Display


#--------------------------------------------------------------------------------------------------
# Widgets:

# Main dialog
if 'pywin' in sys.modules:
    try: root.destroy()
    except: pass
    root = tk.Toplevel() # Use in IDE
else:
    root = tk.Tk()
root.title("Fablight Control")
root.resizable(width=0, height=0)
root.geometry('+10+10') #Toplevel window position
icon = PIL.ImageTk.PhotoImage( PIL.Image.open('fablight.gif') )
root.tk.call('wm', 'iconphoto', root._w, icon)

# Color exchanger
color = Color_Broadcast()

# Color display frame
color_display = Color_Display(root, color)

# Frame: Select device 
dev_frame = tk.LabelFrame(root, text='Device', padx=4, pady=4)
button_open = tk.Button(dev_frame, width=10, text='Open', command=lambda:on_button_open() )
button_send = tk.Button(dev_frame, width=10, text='Send', command=lambda:on_button_send() )
button_recv = tk.Button(dev_frame, width=10, text='Receive', command=lambda:on_button_recv() )
autosend = tk.IntVar(); autosend.set(0)
check_autosend = tk.Checkbutton(dev_frame, text="Autosend", variable=autosend)

port_name = tk.StringVar(); port_name.set("Select port")
port_menu = ttk.Combobox(dev_frame, textvariable=port_name, values=[], width=9 )
port_menu.bind("<Button-1>", lambda(x): on_port_menu(x))
port_menu.bind("<<ComboboxSelected>>", lambda(x):on_port_select(x))


# Color picker tabs
class MyNotebook(ttk.Notebook):
    """ttk.Notebook with small enhancement: A <<NotbookTabChanged>> virtual
    event is forwarded to the selected child tab."""
    def __init__(self, master=None, **kw):
        ttk.Notebook.__init__(self, master, **kw)
        self.bind('<<NotebookTabChanged>>', self.on_tab_changed)
    def on_tab_changed(self, event):
        selected_tab = self.nametowidget( event.widget.select() )
        selected_tab.event_generate('<<NotebookTabChanged>>',data=1234) 

tabs = MyNotebook(root)

# RGB
tab_rgb = ttk.Frame(tabs)
tabs.add(tab_rgb, text="RGB")
rgb_picker = RGB_Picker(tab_rgb, color)

# HSV
tab_hsv = ttk.Frame(tabs)
tabs.add(tab_hsv, text="HSV")
hsv_picker = HSV_Picker(tab_hsv, color)

# H(SV)
tab_hsv2 = ttk.Frame(tabs)
tabs.add(tab_hsv2, text="H(SV)")
hsv2_picker = H_SV_Picker(tab_hsv2, color)

# H(SV)
tab_hsv3 = ttk.Frame(tabs)
tabs.add(tab_hsv3, text="(HS)V")
hsv3_picker = HS_V_Picker(tab_hsv3, color)

### H(CL)
tab_hcl = ttk.Frame(tabs)
tabs.add(tab_hcl, text="H(CL)")
hcl_picker = H_CL_Picker(tab_hcl, color)

#-------------------------------------------------------------------------------------------------
# Configure layout

# Color picker tabs
tabs.grid(column=0, row=0, padx=4, pady=4, sticky=tk.W+tk.E)
color_display.place(column=1, row=0, padx=4, pady=(16,4), sticky=tk.N)

# Device frame
dev_frame.grid(column=0, row=1, padx=4, pady=4, sticky=tk.W+tk.E)
port_menu.grid(column=0, row=0, padx=3, pady=1, sticky=tk.W)
button_open.grid(column=0, row=1, padx=4, pady=2, sticky=tk.W+tk.E)
button_send.grid(column=1, row=0, padx=4, pady=2, sticky=tk.W)
button_recv.grid(column=1, row=1, padx=4, pady=2, sticky=tk.W)
check_autosend.grid(column=2, row=0, padx=2, pady=2, sticky=tk.W)


#--------------------------------------------------------------------------------------------
# GUI event handling

def on_port_menu(event):
    ports = fablightcom.list_ports()
    port_menu.configure(values=ports)

def on_port_select(x):
    print "Port select: ", port_name.get()

def on_button_open():
    if fabcom.is_open():
        fabcom.close()
        button_open.config(text='Open')
        return
    else:
        port = port_name.get()
        if not port or port=='Select port':
            print 'Specify a com port to connect to!'
            return
        print "Open port:", port
        # Note: Put to a thread to prevent GUI from freezing during open
        def open_device(port):
            fabcom.open(port)
            if fabcom.is_open():
                button_open.config(text="Close")
                id = fabcom.get_id()
                print "Connected to:", id
            else:
                print "Error opening port."
        threading.Thread( target=open_device, args=(port,) ).start()

def on_button_send():
    send_color_to_device()

def on_button_receive():
    print "Receive not yet implemented."


# Data transfer handling
def send_color_to_device():
    r, g, b = color.get_rgb()
    if fabcom.is_open():
        fabcom.set_rgbw( 255*r, 255*g, 255*b, 0 )

def autosend_color_to_device():
    r, g, b = color.get_rgb()
    if autosend.get() and fabcom.is_open():
        fabcom.set_rgbw( 255*r, 255*g, 255*b, 0 )
    
color.subscribe( autosend_color_to_device)

#-------------------------------------------------------------------------------------------------
# Global event handler:

# Periodic stdout update (stdout from threads
# is not flushed automatically).
from sys import stdout
def flush_stdout(widget):
    stdout.flush()
    widget.after(100, flush_stdout, widget)
timer_dummy = tk.Label(root)    # (need a widget to connect the timer to)
flush_stdout(timer_dummy)

#-------------------------------------------------------------------------------------------------
# Start GUI
root.mainloop()