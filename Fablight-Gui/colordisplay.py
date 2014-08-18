
from numpy import *

import Tkinter as tk
import ttk
import PIL.Image, PIL.ImageTk

#-----------------------------------------------------------------------------------------
# Color display: Shows current RGB and colorspace specific values
# and a colored patch with target color

class Color_Display:
    patch_size = 64, 64
    patch_img = None
    rgb = 0,0,0  # Stored rgb components in range [0..255] 
    var = ()

    def __init__(self, parent, color_broadcast=None):
        self.parent = parent
        self.frame = tk.LabelFrame(self.parent, text="Target Color", width=40)
        self.colorbc = color_broadcast

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

        # Layout
        self.place()

        # Initial color
        self.receive_color()
        
        # Register for automatic update
        if self.colorbc:
            self.colorbc.subscribe(self.on_color_change)

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

    def on_color_change(self):
        """Registered as callback function to global color broadcast """
        rgb, var = self.colorbc.get()
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

    def set_var(self, var=()):
        """Set up to 3 arbitrary color space parameters to display. Pass as tuple ( (name,value) , ...)"""
        if var==None: var=()
        try:
            for v in var[:3]: v[0], v[1]
        except:
            print '### Color_Display: Error: var must be a tuple ( (name, value), ... ) '
            v = ()
        for i in range(3):
            self.var_labels[i].config( text='' )
            self.var_vals[i].set( '' )
        for i in range(min(3,len(var))):
            self.var_labels[i].config( text=str(var[i][0])+':' )
            self.var_vals[i].set( str(var[i][1])[:6] )

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

    def receive_color(self):
        if self.colorbc:
            rgb, var = self.colorbc.get()
            self.set( rgb, var )

