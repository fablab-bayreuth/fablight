
from numpy import *
from colorsys import *

import Tkinter as tk
import ttk
import PIL.Image, PIL.ImageTk

#-----------------------------------------------------------------------------------------
# HSV picker
# Three horizontal scales for Hue, Sat, Val

class HSV_Picker:
    panel_size = 290, 32
    hue, sat, val = 0, 0, 0
    
    hue_img, sat_img, val_img = None, None, None

    def __init__( self, parent, color_broadcast=None ):
        self.parent = parent
        self.frame = tk.Frame(self.parent)
        self.colorbc = color_broadcast

        # Get initial color
        self.receive_color()
        
        # Create initial images
        self.create_img()

        # setup frames
        self.hue_panel = tk.Label(self.frame, image=self.hue_img, bd=0,
                                  width=self.panel_size[0], height=self.panel_size[1] )
        self.sat_panel = tk.Label(self.frame, image=self.sat_img, bd=0,
                                  width=self.panel_size[0], height=self.panel_size[1] )
        self.val_panel = tk.Label(self.frame, image=self.val_img, bd=0,
                                  width=self.panel_size[0], height=self.panel_size[1] )
        
        # bind event handlers
        self.hue_panel.bind('<Button-1>', self.on_hue_click)
        self.hue_panel.bind('<B1-Motion>', self.on_hue_click)
        self.sat_panel.bind('<Button-1>', self.on_sat_click)
        self.sat_panel.bind('<B1-Motion>', self.on_sat_click)
        self.val_panel.bind('<Button-1>', self.on_val_click)
        self.val_panel.bind('<B1-Motion>', self.on_val_click)
        self.parent.bind('<<NotebookTabChanged>>', self.on_tab_changed)

        self.place()
        
    def place(self,**args): # place frames on grid
        self.frame.grid(args)
        tk.Label(self.frame, text='Hue').grid(column=0, row=0, padx=8, pady=(6,0), sticky=tk.W)
        self.hue_panel.grid(column=0, row=1, padx=8, pady=(0,6), sticky=tk.W+tk.E)
        tk.Label(self.frame, text='Saturation').grid(column=0, row=2, padx=8, pady=0, sticky=tk.W)
        self.sat_panel.grid(column=0, row=3, padx=8, pady=(0,6), sticky=tk.W+tk.E)
        tk.Label(self.frame, text='Value (Brightness)').grid(column=0, row=4, padx=8, pady=0, sticky=tk.W)
        self.val_panel.grid(column=0, row=5, padx=8, pady=(0,6), sticky=tk.W+tk.E)
        ##self.hue_panel.grid(column=0, row=0, padx=8, pady=8, sticky=tk.W+tk.E)
        ##self.sat_panel.grid(column=0, row=1, padx=8, pady=8, sticky=tk.W+tk.E)
        ##self.val_panel.grid(column=0, row=2, padx=8, pady=8, sticky=tk.W+tk.E)

    def create_img(self):
        self.create_hue_img()
        self.create_sat_img()
        self.create_val_img()

    def create_hue_img(self):
        w,h = self.panel_size
        if (self.hue_img==None): # First call, create color scale
            hue_scale = empty((h,w,3), dtype=uint8)
            hue_scale[:] = 255*array([hsv_to_rgb(x,0.9,0.9) for x in 1.*arange(0,w)/w])
            self.hue_scale = hue_scale
        # Mark current value
        hue_scale = self.hue_scale.copy()
        hue_scale[:, int(self.hue*(w-1)), :] = 0
        # Create image object for gui
        hue_img = PIL.Image.frombuffer('RGB', (w,h), hue_scale, 'raw', 'RGB', 0, 1)
        if (self.hue_img==None):
            self.hue_img = PIL.ImageTk.PhotoImage( hue_img )
        else:
            self.hue_img.paste( hue_img ) # PASTE! Do not replace. Image frame remembers original object

    def create_sat_img(self):
        w,h = self.panel_size
        sat_scale = empty((h,w,3), dtype=uint8)
        sat_scale[:] = 255*array([hsv_to_rgb(self.hue, x, 1) for x in 1.*arange(0,w)/w])
        #Mark current value
        sat_scale[:, int(self.sat*(w-1)), :] = 0
        # Create image object for gui
        sat_img = PIL.Image.frombuffer('RGB', (w,h), sat_scale, 'raw', 'RGB', 0, 1)
        if (self.sat_img==None):
            self.sat_img = PIL.ImageTk.PhotoImage( sat_img )
        else:
            self.sat_img.paste( sat_img ) # PASTE! Do not replace. Image frame remembers original object


    def create_val_img(self):
        w,h = self.panel_size
        val_scale = empty((h,w,3), dtype=uint8)
        val_scale[:] = 255*array([hsv_to_rgb(self.hue, self.sat, x) for x in 1.*arange(0,w)/w])
        # Mark current value
        val_scale[:, int(self.val*(w-1)), :] = 255 if self.val<0.5 else 0
        # Create image object for gui
        val_img = PIL.Image.frombuffer('RGB', (w,h), val_scale, 'raw', 'RGB', 0, 1)
        if (self.val_img==None):
            self.val_img = PIL.ImageTk.PhotoImage( val_img )
        else:
            self.val_img.paste( val_img ) # PASTE! Do not replace. Image frame remembers original object


    def on_hue_click(self, event):
        x = clip( event.x, 0, self.panel_size[0] )
        print 'x=', x
        self.hue = float(x)/self.panel_size[0]
        print "hue=", self.hue
        self.create_hue_img()        
        self.create_sat_img()
        self.create_val_img()
        self.broadcast_color()

    def on_sat_click(self, event):
        x = clip( event.x, 0, self.panel_size[0] )
        print 'x=', x
        self.sat = float(x)/self.panel_size[0]
        print "sat=", self.sat
        self.create_sat_img()
        self.create_val_img()
        self.broadcast_color()

    def on_val_click(self, event):
        x = clip( event.x, 0, self.panel_size[0] )
        print 'x=', x
        self.val = float(x)/self.panel_size[0]
        print "val=", self.val
        self.create_sat_img()
        self.create_val_img()
        self.broadcast_color()

    def on_tab_changed(self, event):
        print 'HSV tab'
        self.receive_color()
        self.create_img()
        self.broadcast_color()

    def broadcast_color(self):
        if self.colorbc:
            rgb = hsv_to_rgb(self.hue, self.sat, self.val)
            var = ( ('H',self.hue), ('S',self.sat), ('V',self.val) )
            self.colorbc.set( rgb, var )

    def receive_color(self):
        if self.colorbc:
            r,g,b = self.colorbc.get_rgb()
        else: r,g,b = 0,0,0
        self.hue, self.sat, self.val = rgb_to_hsv(r,g,b)

#-----------------------------------------------------------------------------------------
# H(SV) picker
# Two widgets: sat-val plane, vertical hue scale 

class H_SV_Picker:
    hue_panel_size = 32, 256
    sv_panel_size = 256, 256  
    hue, sat, val = 0, 0, 0

    hue_img = None
    sv_img = None

    def __init__(self, parent, color_broadcast=None):
        self.parent = parent
        self.frame = tk.Frame(self.parent)
        self.colorbc = color_broadcast

        # Get initial color
        self.receive_color()
        
        # Create initial images
        self.create_img()

        # setup frames
        self.sv_panel = tk.Label(self.frame, image=self.sv_img, bd=0,
                                 width=self.sv_panel_size[0], height=self.sv_panel_size[1])
        self.hue_panel = tk.Label(self.frame, image=self.hue_img, bd=0,
                                  width=self.hue_panel_size[0], height=self.hue_panel_size[1] )
        
        # bind event handlers
        self.sv_panel.bind('<Button-1>', self.on_sv_click)
        self.sv_panel.bind('<B1-Motion>', self.on_sv_click)
        self.hue_panel.bind('<Button-1>', self.on_hue_click)
        self.hue_panel.bind('<B1-Motion>', self.on_hue_click)
        self.parent.bind('<<NotebookTabChanged>>', self.on_tab_changed)

        self.place()

    def place(self, **args): # place frames on grid
        self.frame.grid(args)
        tk.Label(self.frame, text='Saturation / Value (Brightness)').grid(column=0, row=0, padx=(8,4), pady=(4,0), sticky=tk.W)
        self.sv_panel.grid(column=0, row=1, padx=(8,4), pady=(2,8), sticky=tk.W+tk.E+tk.N+tk.S)
        tk.Label(self.frame, text='Hue').grid(column=1, row=0, padx=(4,8), pady=(4,0))
        self.hue_panel.grid(column=1, row=1, padx=(4,8), pady=(2,8), sticky=tk.N+tk.S)

    def create_hue_img(self):
        w,h = self.hue_panel_size
        if (self.hue_img==None): # First call, create static hue-scale
            hue_scale = 255*array([hsv_to_rgb(1.-y,0.9,0.9) for y in 1.*arange(0,h)/h])
            self.hue_scale = hue_scale.astype(uint8).copy() # Make sure to keep a copy!
        hue_scale = self.hue_scale.copy()
        hue_scale[ int((1-self.hue)*(h-1)), :]=0  # Mark current hue value
        hue_img = PIL.Image.frombuffer('RGB', (1,h), hue_scale, 'raw', 'RGB', 0, 1)
        hue_img = hue_img.resize( self.hue_panel_size )
        if (self.hue_img==None):
            self.hue_img = PIL.ImageTk.PhotoImage( hue_img )
        else:
            self.hue_img.paste( hue_img ) # PASTE! Do not replace. Image frame remembers original object

    def create_sv_img(self, hue=None):
        if (hue==None): hue=self.hue
        rgb = array(hsv_to_rgb(self.hue,1,1))
        # Create sat axis for given hue
        ##for si in range(256): sat[si,256] = (rgb-1)*si/256.+1
        sat = (full((256,3), rgb )-1) * tile( arange(256)[:,newaxis], (1,3))/256. + 1
        # Create sat-val plane from sat axis
        ##for vi in range(256): fd1[:,vi] = fd1[:,256] *vi/256.
        sv = transpose( tile(sat[:,newaxis], (256,1) ), (1,0,2) )* repeat(arange(256)[::-1],256*3).reshape(256,256,3)/256.
        # Mark click position
        s,v = int(self.sat*255), int(255-self.val*255)
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
        y = clip( event.y, 0, self.hue_panel_size[1] )
        print 'y=', y
        self.hue = 1.-float(y)/self.hue_panel_size[1]
        print "hue=", self.hue
        self.create_hue_img()
        self.create_sv_img()
        self.broadcast_color()

    def on_sv_click(self, event):
        x = clip( event.x, 0, self.sv_panel_size[0] )
        y = clip( event.y, 0, self.sv_panel_size[1] )
        self.sat = float(x)/self.sv_panel_size[0]
        self.val = 1.-float(y)/self.sv_panel_size[1]
        print "sat=", self.sat, "val=", self.val
        self.create_sv_img()
        self.broadcast_color()

    def on_tab_changed(self, event):
        print 'H(SV) tab'
        self.receive_color()
        self.create_img()
        self.broadcast_color()
        
    def broadcast_color(self):
        if self.colorbc:
            rgb = hsv_to_rgb(self.hue, self.sat, self.val)
            var = ( ('H',self.hue), ('S',self.sat), ('V',self.val) )
            self.colorbc.set( rgb, var )

    def receive_color(self):
        if self.colorbc:
            r,g,b = self.colorbc.get_rgb()
        else: r,g,b = 0,0,0
        self.hue, self.sat, self.val = rgb_to_hsv(r,g,b)


#-----------------------------------------------------------------------------------------
# (HS)V picker
# Two widgets: hue-sat plane, vertical val scale 

class HS_V_Picker:
    hs_panel_size = 256, 256
    val_panel_size = 32, 256  
    hue, sat, val = 0, 0, 0

    hs_img = None
    val_img = None

    def __init__(self, parent, color_broadcast=None):
        self.parent = parent
        self.frame = tk.Frame(self.parent)
        self.colorbc = color_broadcast

        # Get initial color
        self.receive_color()
        
        # Create initial images
        self.create_img()

        # setup frames
        self.hs_panel = tk.Label(self.frame, image=self.hs_img, bd=0,
                                 width=self.hs_panel_size[0], height=self.hs_panel_size[1])
        self.val_panel = tk.Label(self.frame, image=self.val_img, bd=0,
                                  width=self.val_panel_size[0], height=self.val_panel_size[1] )
        
        # bind event handlers
        self.hs_panel.bind('<Button-1>', self.on_hs_click)
        self.hs_panel.bind('<B1-Motion>', self.on_hs_click)
        self.val_panel.bind('<Button-1>', self.on_val_click)
        self.val_panel.bind('<B1-Motion>', self.on_val_click)
        self.parent.bind('<<NotebookTabChanged>>', self.on_tab_changed)

        self.place()

    def place(self, **args): # place frames on grid
        self.frame.grid(args)
        tk.Label(self.frame, text='Hue / Saturation').grid(column=0, row=0, padx=(8,4), pady=(4,0), sticky=tk.W)
        self.hs_panel.grid(column=0, row=1, padx=(8,4), pady=(2,8), sticky=tk.W+tk.E+tk.N+tk.S)
        tk.Label(self.frame, text='Value').grid(column=1, row=0, padx=(4,8), pady=(4,0))
        self.val_panel.grid(column=1, row=1, padx=(4,8), pady=(2,8), sticky=tk.N+tk.S)

    def create_val_img(self):
        w,h = self.val_panel_size
        if (self.val_img==None): # First call, create static val-scale
            val_scale = repeat(1.0*arange(h)[::-1]/h, w*3).reshape((h,w,3))
            self.val_scale = (255*val_scale).astype(uint8)
        val_scale = self.val_scale.copy()
        rgb = hsv_to_rgb(self.hue, self.sat, 0.9)
        #val_scale *= rgb
        val_scale = (val_scale.astype(float) * rgb).astype(uint8)
        # Mark current value
        val_scale[ int((1-self.val)*(h-1)), :, :] = 255 if self.val<0.5 else 0
        # Create image object for gui
        val_img = PIL.Image.frombuffer('RGB', (w,h), val_scale, 'raw', 'RGB', 0, 1)
        if (self.val_img==None):
            self.val_img = PIL.ImageTk.PhotoImage( val_img )
        else:
            self.val_img.paste( val_img ) # PASTE! Do not replace. Image frame remembers original object

    def create_hs_img(self):
        w,h = self.hs_panel_size
        if (self.hs_img==None): # First call, create static hue-sat-scale
            hue_scale = array([hsv_to_rgb(x,1,1) for x in 1.*arange(0,w)/w])
            sat_weight = repeat(1.0*arange(h)[::-1]/h, w*3).reshape((h,w,3))
            hs_scale = tile( hue_scale, (h,1) ).reshape(h,w,3)
            hs_scale = (hs_scale-1)*sat_weight+1
            self.hs_scale = (255*hs_scale).astype(uint8)
        hs_scale = self.hs_scale.copy()
        # Mark click position
        x,y = int(self.hue*255), int(255-self.sat*255)
        x0=max(x-10,0); x1=min(x+10,255); y0=max(y-10,0); y1=min(y+10,255)
        c = 1. if y>100 else 0. 
        hs_scale[y,x0:x1,:] = c; hs_scale[y0:y1,x,:] = c
        # Create image object from data
        hs_img = PIL.Image.frombuffer('RGB', (256,256), hs_scale, 'raw', 'RGB', 0, 1)
        if (self.hs_img==None):
            self.hs_img = PIL.ImageTk.PhotoImage( hs_img ) # Make sure to keep a copy!
        else:
            self.hs_img.paste( hs_img ) # PASTE! Do not replace. Image frame remembers original object

    def create_img(self):
        self.create_hs_img()
        self.create_val_img()

    def on_val_click(self, event):
        y = clip( event.y, 0, self.val_panel_size[1] )
        print 'y=', y
        self.val = 1.-float(y)/self.val_panel_size[1]
        print "val=", self.val
        self.create_val_img()
        self.broadcast_color()

    def on_hs_click(self, event):
        w,h = self.hs_panel_size
        x = clip( event.x, 0, w )
        y = clip( event.y, 0, h )
        self.hue = float(x)/w
        self.sat = 1.-float(y)/h
        print "hue=", self.hue, "sat=", self.sat
        self.create_hs_img()
        self.create_val_img()        
        self.broadcast_color()

    def on_tab_changed(self, event):
        print '(HS)V tab'
        self.receive_color()
        self.create_img()
        self.broadcast_color()
        
    def broadcast_color(self):
        if self.colorbc:
            rgb = hsv_to_rgb(self.hue, self.sat, self.val)
            var = ( ('H',self.hue), ('S',self.sat), ('V',self.val) )
            self.colorbc.set( rgb, var )

    def receive_color(self):
        if self.colorbc:
            r,g,b = self.colorbc.get_rgb()
        else: r,g,b = 0,0,0
        self.hue, self.sat, self.val = rgb_to_hsv(r,g,b)
