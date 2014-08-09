from numpy import *
from colorsys import *

import Tkinter as tk
import ttk
import PIL.Image, PIL.ImageTk

import fablightcom
fabcom = fablightcom.FablightCom()

#-----------------------------------------------------------------------------------------
# Color display: Shows current RGB and colorspace specific values
# and a colored patch with target color

class Color_Display:
    patch_size = 64, 64
    patch_img = None
    rgb = 0,0,0  # Stored rgb components in range [0..255] 
    var = {}

    def __init__(self, parent):
        self.parent = parent
        self.frame = tk.LabelFrame(self.parent, text="Target Color", width=40)

        # Color patch 
        self.create_patch_img()
        self.patch = tk.Label(self.frame, image=self.patch_img,
                                 width=self.patch_size[0], height=self.patch_size[1])

        # RGB Labels
        self.r_val = tk.StringVar()
        self.g_val = tk.StringVar()
        self.b_val = tk.StringVar()        
        self.rgb_frame = tk.Frame(self.frame)
        self.r_label = tk.Label(self.rgb_frame, text='R: ')
        self.g_label = tk.Label(self.rgb_frame, text='G: ')
        self.b_label = tk.Label(self.rgb_frame, text='B: ')
        self.r_box = tk.Label(self.rgb_frame, textvariable=self.r_val)
        self.g_box = tk.Label(self.rgb_frame, textvariable=self.g_val)
        self.b_box = tk.Label(self.rgb_frame, textvariable=self.b_val)

        self.sep = ttk.Separator(self.frame, orient=tk.HORIZONTAL, )
        
        # Model specific Labels
        self.var_frame = tk.Frame(self.frame)
        self.var_vals = [tk.StringVar() for i in range(3)]
        self.var_labels = [tk.Label(self.var_frame) for i in range(3) ]
        self.var_boxes = [tk.Label(self.var_frame,
                     textvariable=self.var_vals[i]) for i in range(3)]

        # Initial color
        rgb, var = color.get()
        self.set( rgb, var )

        # Register for automatic update
        color.subscribe(self.on_color_change)

    def on_color_change(self):
        """Registered as callback function to global color broadcast """
        rgb, var = color.get()
        self.set( rgb, var )

    def set_rgb(self, rgb=None):
        """Set patch color in rgb parameters"""
        if (rgb!=None):
            rgb = [i*255 for i in rgb[0:3]]
            self.rgb = rgb
        self.r_val.set( str(int(self.rgb[0])) )
        self.g_val.set( str(int(self.rgb[1])) )
        self.b_val.set( str(int(self.rgb[2])) )
        self.create_patch_img()

    def set_var(self, var={}):
        """Set up to 3 arbitrary color space parameters to display. Pass as ditionary {name:value, ...}"""
        if var==None: var={}
        self.var = var
        labels, values = ['','',''],['','','']
        keys = var.keys()
        try:
            for i in range(3):
               labels[i] = keys[i]; values[i] = var[keys[i]]
        except: pass 
        for i in range(3):
            if labels[i]: labels[i] += ':'
            self.var_labels[i].config( text=str(labels[i]) )
            self.var_vals[i].set( str(values[i])[:6] )

    def set(self, rgb=None, var={}):
        self.set_rgb(rgb)
        self.set_var(var)

    def create_patch_img(self):
        d = full( self.patch_size+(3,), self.rgb, dtype=uint8 ) # uniform array from rgb values        
        patch_img = PIL.Image.frombuffer('RGB', self.patch_size, d, 'raw', 'RGB', 0, 1)
        if (self.patch_img==None):
            self.patch_img = PIL.ImageTk.PhotoImage( patch_img )
        else:
            self.patch_img.paste( patch_img ) # PASTE! Do not replace. Image frame remembers original object

    def place(self, **args):
        """Place widgets on grid. Call with grid parameters for enclosing frame"""
        self.frame.grid(args)
        self.patch.grid(column=0, row=0, padx=4, pady=4)
        self.rgb_frame.grid(column=0, row=1, padx=(8,4), pady=2, sticky=tk.W)
        self.sep.grid(column=0, row=2, sticky=tk.E+tk.W)
        self.var_frame.grid(column=0, row=3, padx=(8,4), pady=2, sticky=tk.W)        
        self.r_label.grid(column=0, row=0, sticky=tk.W); self.r_box.grid(column=1, row=0, sticky=tk.W)
        self.g_label.grid(column=0, row=1, sticky=tk.W); self.g_box.grid(column=1, row=1, sticky=tk.W)
        self.b_label.grid(column=0, row=2, sticky=tk.W); self.b_box.grid(column=1, row=2, sticky=tk.W)
        for i in range(3):
            self.var_labels[i].grid(column=0, row=i, sticky=tk.W)
            self.var_boxes[i].grid(column=1, row=i, sticky=tk.W)


#-----------------------------------------------------------------------------------------
# HSV picker
# Two widgets: sat-val plane, vertical hue scale 

class HSV_Picker:
    hue_panel_size = 32, 256
    sv_panel_size = 256, 256  
    hue, sat, val = 0, 0, 0

    hue_img = None
    sv_img = None

    def __init__(self,parent):
        self.parent = parent
        self.frame = tk.Frame(self.parent)

        # Get initial color
        r,g,b = color.get_rgb()
        self.hue, self.sat, self.val = rgb_to_hsv(r,g,b)

        # Create initial images
        self.create_img()

        # setup frames
        self.sv_panel = tk.Label(self.frame, image=self.sv_img, bd=0,
                                 width=self.sv_panel_size[0], height=self.sv_panel_size[1])
        self.hue_panel = tk.Label(self.frame, image=self.hue_img, bd=0,
                                  width=self.hue_panel_size[0], height=self.sv_panel_size[1] )
        
        # bind event handlers
        self.sv_panel.bind('<Button-1>', self.on_sv_click)
        self.sv_panel.bind('<B1-Motion>', self.on_sv_click)
        self.hue_panel.bind('<Button-1>', self.on_hue_click)
        self.hue_panel.bind('<B1-Motion>', self.on_hue_click)
        self.parent.bind('<<NotebookTabChanged>>', self.on_tab_changed)

        self.place()

    def place(self, **args): # place frames on grid
        self.frame.grid(args)
        self.sv_panel.grid(column=0, row=0, padx=4, pady=4, sticky=tk.W+tk.E+tk.N+tk.S)
        self.hue_panel.grid(column=1, row=0, padx=4, pady=4, sticky=tk.N+tk.S)

    def create_hue_img(self):
        if (self.hue_img==None): # First call, create static hue-scale
            hue_scale = 255*array([hsv_to_rgb(1.-h,0.9,0.9) for h in arange(0,256)/256.])
            self.hue_scale = hue_scale.astype(uint8).copy() # Make sure to keep a copy!
        hue_scale = self.hue_scale.copy()
        hue_scale[255-self.hue*255,:]=0  # Mark current hue value
        hue_img = PIL.Image.frombuffer('RGB', (1,256), hue_scale, 'raw', 'RGB', 0, 1)
        hue_img = hue_img.resize( self.hue_panel_size )
        if (self.hue_img==None):
            self.hue_img = PIL.ImageTk.PhotoImage( hue_img )
        else:
            self.hue_img.paste( hue_img ) # PASTE! Do not replace. Image frame remembers original object

    def create_sv_img(self, hue=None):
        if (hue==None): hue=self.hue
        rgb = array(hsv_to_rgb(self.hue,1,1))
        # Create sat axis for given hue
        ##for si in range(256): sat[si,256] = (x-1)*si/256.+1
        sat = (full((256,3), rgb )-1) * tile( arange(256)[:,newaxis], (1,3))/256. + 1
        # Create sat-val plane from sat axis
        ##for vi in range(256): fd1[:,vi] = fd1[:,256] *vi/256.
        sv = transpose( tile(sat[:,newaxis], (256,1) ), (1,0,2) )* repeat(arange(256)[::-1],256*3).reshape(256,256,3)/256.
        # Mark click position
        s,v = self.sat*255, 255-self.val*255
        s0=max(s-10,0); s1=min(s+10,255); v0=max(v-10,0); v1=min(v+10,255)
        c = 1. if v>100 else 0. 
        sv[v,s0:s1,:] = c; sv[v0:v1,s,:] = c
        # Create image object from data
        sv_img = PIL.Image.frombuffer('RGB', (256,256), (sv*255).astype(uint8), 'raw', 'RGB', 0, 1)
        sv_img = sv_img.resize( self.sv_panel_size )
        if (self.sv_img==None):
            self.sv_img = PIL.ImageTk.PhotoImage( sv_img ) # Make sure to keep a copy!
        else:
            self.sv_img.paste( sv_img ) # PASTE! Do not replace. Image frame remembers original object

    def create_img(self):
        self.create_hue_img()
        self.create_sv_img()

    def on_hue_click(self, event):
        y = clip( event.y, 0, self.sv_panel_size[1] )
        print 'y=', y
        self.hue = 1.-float(y)/self.hue_panel_size[1]
        print "hue=", self.hue
        self.create_hue_img()
        self.create_sv_img()
        self.broadcast_change()

    def on_sv_click(self, event):
        x = clip( event.x, 0, self.sv_panel_size[0] )
        y = clip( event.y, 0, self.sv_panel_size[1] )
        self.sat = float(x)/self.sv_panel_size[0]
        self.val = 1.-float(y)/self.sv_panel_size[1]
        print "sat=", self.sat, "val=", self.val
        self.create_sv_img()
        self.broadcast_change()

    def on_tab_changed(self, event):
        print 'HSV tab'
        rgb = color.get_rgb()
        self.hue, self.sat, self.val = rgb_to_hsv(*rgb)
        self.create_img()
        self.broadcast_change()
        
    def broadcast_change(self):
        rgb = hsv_to_rgb(self.hue, self.sat, self.val)
        var = {'H':self.hue, 'S':self.sat, 'V':self.val }
        color.set( rgb, var )

#-----------------------------------------------------------------------------------------
# RGB picker
# Three faders for Red/Green/Blue components 

class RGB_Picker:
    rgb_panel_size = 290, 32
    r,g,b = 0,0,0
    r_img, g_img, b_img = None, None, None

    def __init__(self,parent):
        self.parent = parent
        self.frame = tk.Frame(self.parent)

        # Initial color
        self.rgb = color.get_rgb()

        # Create initial images
        self.create_rgb_img()
        
        # setup frame
        self.r_panel = tk.Label(self.frame, image=self.r_img, bd=0,
                            width=self.rgb_panel_size[0], height=self.rgb_panel_size[1])
        self.g_panel = tk.Label(self.frame, image=self.g_img, bd=0,
                            width=self.rgb_panel_size[0], height=self.rgb_panel_size[1] )
        self.b_panel = tk.Label(self.frame, image=self.b_img, bd=0,
                            width=self.rgb_panel_size[0], height=self.rgb_panel_size[1] )
        
        # bind event handlers
        self.r_panel.bind('<Button-1>', self.on_r_click)
        self.r_panel.bind('<B1-Motion>', self.on_r_click)
        self.g_panel.bind('<Button-1>', self.on_g_click)
        self.g_panel.bind('<B1-Motion>', self.on_g_click)
        self.b_panel.bind('<Button-1>', self.on_b_click)
        self.b_panel.bind('<B1-Motion>', self.on_b_click)
        self.parent.bind('<<NotebookTabChanged>>', self.on_tab_changed)

        self.place()
        
    def place(self,**args): # place frames on grid
        self.frame.grid(args)
        self.r_panel.grid(column=0, row=0, padx=8, pady=8, sticky=tk.W+tk.E)
        self.g_panel.grid(column=0, row=1, padx=8, pady=8, sticky=tk.W+tk.E)
        self.b_panel.grid(column=0, row=2, padx=8, pady=8, sticky=tk.W+tk.E)

    def create_rgb_img(self):
        self.create_r_img()
        self.create_g_img()
        self.create_b_img()

    def create_r_img(self):
        w,h = self.rgb_panel_size
        if (self.r_img==None): # First call, create color scale
            self.r_scale = zeros((h,w,3), dtype=uint8)
            self.r_scale[:,:,0] = 255./w*arange(w)
        # Mark current value
        r_scale = self.r_scale.copy()
        r_scale[:,self.r*(w-1),:] = 255
        # Create image object for gui
        r_img = PIL.Image.frombuffer('RGB', (w,h), r_scale, 'raw', 'RGB', 0, 1)
        if (self.r_img==None):
            self.r_img = PIL.ImageTk.PhotoImage( r_img )
        else:
            self.r_img.paste( r_img ) # PASTE! Do not replace. Image frame remembers original object

    def create_g_img(self):
        w,h = self.rgb_panel_size
        if (self.g_img==None): # First call, create color scales
            self.g_scale = zeros((h,w,3), dtype=uint8)
            self.g_scale[:,:,1] = 255./w*arange(w)
        # Mark current values
        g_scale = self.g_scale.copy()
        g_scale[:,self.g*(w-1),:] = 255
        # Create image objects for gui
        g_img = PIL.Image.frombuffer('RGB', (w,h), g_scale, 'raw', 'RGB', 0, 1)
        if (self.g_img==None):
            self.g_img = PIL.ImageTk.PhotoImage( g_img )
        else:            
            self.g_img.paste( g_img ) # PASTE! Do not replace. Image frame remembers original object

    def create_b_img(self):
        w,h = self.rgb_panel_size
        if (self.b_img==None): # First call, create color scales
            self.b_scale = zeros((h,w,3), dtype=uint8)
            self.b_scale[:,:,2] = 255./w*arange(w)
        # Mark current values
        b_scale = self.b_scale.copy()
        b_scale[:,self.b*(w-1),:] = 255
        # Cerate image objects for gui
        b_img = PIL.Image.frombuffer('RGB', (w,h), b_scale, 'raw', 'RGB', 0, 1)
        if (self.b_img==None):
            self.b_img = PIL.ImageTk.PhotoImage( b_img )            
        else:
            self.b_img.paste( b_img ) # PASTE! Do not replace. Image frame remembers original object

    def on_r_click(self, event):
        x = clip( event.x, 0, self.rgb_panel_size[0] )
        print 'x=', x
        self.r = float(x)/self.rgb_panel_size[0]
        print "r=", self.r
        self.create_r_img()
        self.broadcast_change()

    def on_g_click(self, event):
        x = clip( event.x, 0, self.rgb_panel_size[0] )
        print 'x=', x
        self.g = float(x)/self.rgb_panel_size[0]
        print "g=", self.g
        self.create_g_img()
        self.broadcast_change()

    def on_b_click(self, event):
        x = clip( event.x, 0, self.rgb_panel_size[0] )
        print 'x=', x
        self.b = float(x)/self.rgb_panel_size[0]
        print "b=", self.b
        self.create_b_img()
        self.broadcast_change()

    def on_tab_changed(self, event):
        print 'RGB tab'
        self.r, self.g, self.b = color.get_rgb()
        self.create_rgb_img()
        self.broadcast_change()

    def broadcast_change(self):
        rgb = self.r, self.g, self.b
        var = {'R':self.r, 'G':self.g, 'B':self.b}
        color.set( rgb, var )

#-------------------------------------------------------------------------------------------------
# Global color storage
# In a gui environemnt, the proper way to interchange color information
# would be by events/messages. Unfortunately, the python Tk interface
# does not implement data payload for Tk events.
# As a workaround, we use here a global color broadcaster object to exchange
# color data.

class Color_Broadcast:
    """Global color storage and exchange"""
    rgb = 0,0,0  # Reg/Green/Blue components as float [0..1]
    var = None   # Model specific components
    callbacks = None  # List of registered callback functions 
    def __init__(self):
        self.callbacks = []
    def set(self, rgb, var=None):
        """Set new color. rgb components are required, optionally, model
        specific components can be passed as dictionary {name:value, } in var."""
        self.rgb = float(rgb[0]), float(rgb[1]), float(rgb[2])
        if not isinstance(var, dict): raise ValueError('<var> is not a dictionary')
        self.var = var
        for func in self.callbacks: func()
    def get(self):
        """Get current color. Returns tuple (rgb, var)"""
        return self.rgb, self.var
    def get_rgb(self):
        """Get current color rgb components"""
        return self.rgb
    def subscribe(self, func):
        """Register callback function to be called when the color is changed."""
        for f in self.callbacks:
            if f is func: return
        self.callbacks.append(func)
    def unsubscribe(self, func):
        """Unregister a previously registered callback function."""
        for f in reversed(self.callbacks):
            if f is func: self.callbacks.remove(f)

color = Color_Broadcast()

#--------------------------------------------------------------------------------------------------
# Widgets:

# Main dialog
root = tk.Tk()
##root = tk.Toplevel() # Use in IDE
root.title("Fablight Control")
root.resizable(width=0, height=0)
root.geometry('+10+10') # Toplevel window position


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
tab_rgb = ttk.Frame(tabs)
tabs.add(tab_rgb, text="RGB")
##tabs.add(tab_hsv1, text="HSV")
tab_hsv = ttk.Frame(tabs)
tabs.add(tab_hsv, text="H(SV)")
rgb_picker = RGB_Picker(tab_rgb)
hsv_picker = HSV_Picker(tab_hsv)

color_display = Color_Display(root)

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
        fabcom.open(port)
        if fabcom.is_open():
            button_open.config(text="Close")
        else:
            print "Error opening port."

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