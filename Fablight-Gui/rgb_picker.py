
from numpy import *
from colorsys import *

import Tkinter as tk
import ttk
import PIL.Image, PIL.ImageTk

#-----------------------------------------------------------------------------------------
# RGB picker
# Three faders for Red/Green/Blue components 

class RGB_Picker:
    rgb_panel_size = 290, 32
    r,g,b = 0,0,0
    r_img, g_img, b_img = None, None, None

    def __init__(self, parent, color_broadcast=None):
        self.parent = parent
        self.frame = tk.Frame(self.parent)
        self.colorbc = color_broadcast

        # Initial color
        self.receive_color()

        # Create initial images
        self.create_img()
        
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
        tk.Label(self.frame, text='Red').grid(column=0, row=0, padx=8, pady=(6,0), sticky=tk.W)
        self.r_panel.grid(column=0, row=1, padx=8, pady=(0,6), sticky=tk.W+tk.E)
        tk.Label(self.frame, text='Green').grid(column=0, row=2, padx=8, pady=0, sticky=tk.W)
        self.g_panel.grid(column=0, row=3, padx=8, pady=(0,6), sticky=tk.W+tk.E)
        tk.Label(self.frame, text='Blue').grid(column=0, row=4, padx=8, pady=0, sticky=tk.W)
        self.b_panel.grid(column=0, row=5, padx=8, pady=(0,6), sticky=tk.W+tk.E)
        ##self.r_panel.grid(column=0, row=0, padx=8, pady=8, sticky=tk.W+tk.E)
        ##self.g_panel.grid(column=0, row=1, padx=8, pady=8, sticky=tk.W+tk.E)
        ##self.b_panel.grid(column=0, row=2, padx=8, pady=8, sticky=tk.W+tk.E)

    def create_img(self):
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
        r_scale[:, int(self.r*(w-1)), :] = 255
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
        g_scale[:, int(self.g*(w-1)), :] = 255
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
        b_scale[:, int(self.b*(w-1)), :] = 255
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
        self.broadcast_color()

    def on_g_click(self, event):
        x = clip( event.x, 0, self.rgb_panel_size[0] )
        print 'x=', x
        self.g = float(x)/self.rgb_panel_size[0]
        print "g=", self.g
        self.create_g_img()
        self.broadcast_color()

    def on_b_click(self, event):
        x = clip( event.x, 0, self.rgb_panel_size[0] )
        print 'x=', x
        self.b = float(x)/self.rgb_panel_size[0]
        print "b=", self.b
        self.create_b_img()
        self.broadcast_color()

    def on_tab_changed(self, event):
        print 'RGB tab'
        self.receive_color()
        self.create_img()
        self.broadcast_color()

    def broadcast_color(self):
        if self.colorbc:
            rgb = self.r, self.g, self.b
            var = ( ('R',self.r), ('G',self.g), ('B',self.b) )
            self.colorbc.set( rgb, var )

    def receive_color(self):
        if self.colorbc:
            self.r, self.g, self.b = self.colorbc.get_rgb()
        else: self.r, self.g, self.b = 0, 0, 0
        
