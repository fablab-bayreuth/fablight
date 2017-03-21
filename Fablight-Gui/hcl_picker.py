
from numpy import *
from colorsys import *

import Tkinter as tk
import ttk
import PIL.Image, PIL.ImageTk

ir = lambda x: int(round(x))

#-----------------------------------------------------------------------------------------
# H(CL) picker

class H_CL_Picker:
    hue_panel_size = 256, 256
    r0, r1 = 0.4*hue_panel_size[0], 0.5*hue_panel_size[0]  # Hue circle radii 
    rt = r0-5   # CL-Triangle outer radius
    hue_img, cl_img = None, None
    hue, sat, val = 0, 0, 0
    
    def __init__( self, parent, color_broadcast=None ):
        self.parent = parent
        self.frame = tk.Frame(self.parent)
        self.colorbc = color_broadcast

        # Get initial color
        self.receive_color()

        # setup frames
        self.canvas = tk.Canvas(self.frame, bd=-2, width=self.hue_panel_size[0], height=self.hue_panel_size[1] )

        # Create initial images
        self.draw()

        # bind event handlers
        self.canvas.bind('<Button-1>', self.on_hue_click)
        self.canvas.bind('<B1-Motion>', self.on_hue_click)
        self.parent.bind('<<NotebookTabChanged>>', self.on_tab_changed)

        self.place()

    def place(self, **args): # place frames on grid
        self.frame.grid(args)
        self.canvas.grid(column=0, row=0, padx=(12,12), pady=(12,12), sticky=tk.N+tk.S)

    def draw(self):
        self.draw_hue()
        self.draw_cl()

    def draw_hue(self):
        W,H = self.hue_panel_size
        r0,r1 = self.r0, self.r1 
        xm,ym = W/2.,H/2.

        if (self.hue_img==None): # First call, create static hue-image
            d = 255*array([hsv_to_rgb(1.-x,0.9,0.9) for x in arange(256)/255.])
            self.hue_data = d.copy()
            hue_scale = zeros((H,W,4), dtype=uint8)

            # Draw hue circle. THIS IS VERY SLOW!
            for y in range(int(ym),H):
                clip = lambda x,a,b: a if x<a else (b if x>b else x)
                if y-ym>= r0: x0 = xm
                else: x0 = clip( xm + sqrt(r0**2-(y-ym)**2), xm, W-1 )
                if y-ym>= r1: continue
                else: x1 = clip( xm + sqrt(r1**2-(y-ym)**2), xm, W-1 )
                for x in range(int(x0), int(x1)):
                    p = arctan2( y-ym, x-xm )/(2*pi)
                    hue_scale[y,x] = r_[ d[int(255*p)], 255 ]
                    hue_scale[H-1-y,x] = r_[ d[int(255*(1-p))], 255 ]
                    hue_scale[y,W-1-x] = r_[ d[int(255*(0.5-p))], 255 ]
                    hue_scale[H-1-y,W-1-x] = r_[ d[int(255*(0.5+p))], 255 ]
            hue_img = PIL.Image.frombuffer('RGBA', (W,H), hue_scale, 'raw', 'RGBA', 0, 1)
            self.hue_img = PIL.ImageTk.PhotoImage( hue_img )
            self.canvas.create_image( 0,0, anchor=tk.NW, image=self.hue_img, tag='hue_img' )

            phi = self.hue*2*pi
            self.hue_marker = self.canvas.create_line(xm+r0*cos(phi), ym+r0*sin(phi),
                                                         xm+r1*cos(phi), ym+r1*sin(phi))
            self.canvas.tag_bind('hue_img', '<Button-1>', self.foo_hue)
        else:
            phi = -self.hue*2*pi
            self.canvas.coords( self.hue_marker, xm+r0*cos(phi), ym+r0*sin(phi),
                                                xm+r1*cos(phi), ym+r1*sin(phi) )
    def foo_hue(self, event):
        print 'Fooo Hue'


    def draw_cl(self):
        W,H = self.hue_panel_size
        ro = self.rt ## triangle outer radius
        xm,ym = W/2.,H/2.
        a = ir(sqrt(3.)*ro)  ## Triangle side-length
        ri = ir(0.5*ro)      ## Triangle inner radius
        bw = ir(2*ro)   ## width of bounding box

        print 'a=', a, 'bw-ri=', bw-ri
        
        if (self.cl_img==None):
            # Create triangle mask
            cl_mask = zeros( (bw, bw), dtype=uint8)
            for x in arange( int(ri), int(bw) ):
                h = a/(3*ro)*(bw-x)
                for y in arange( int(round(0.5*bw-h)), int(round(0.5*bw+h)) ):
                    cl_mask[y,x] = 255
            self.cl_mask = cl_mask
            
        # Create c-l-triangle ## SLOW ##
        ##cl_data = zeros( (bw, bw, 4), dtype=uint8)
        ##for x in arange( ri, bw ):
        ##    h = a/(3*ro)*(bw-x)
        ##    for y in arange( round(0.5*bw-h), round(0.5*bw+h) ):
        ##        cl_data[y,x] = r_[ self.hue_data[255*(1.-self.hue)], 255 ]

        # Significantly faster, but somewhat cryptic
        rgb = array(hls_to_rgb(self.hue,0.5,1))
        # Create sat axis for given hue
        ##for si in range(256): sat[si,256] = (rgb-1)*si/256.+1
        sat = (full((bw-ri,3), rgb )-1) * tile( arange(bw-ri)[:,newaxis], (1,3))/(1.*(bw-ri)) + 1
        # Create sat-val plane from sat axis
        ##for vi in range(256): fd1[:,vi] = fd1[:,256] *vi/256.
        sv = transpose( tile(sat[:,newaxis], (a,1) ), (1,0,2) )* ( repeat(arange(a)[::-1],(bw-ri)*3).reshape(a,(bw-ri),3)/(1.*a) )

        cl_data = empty( (bw, bw, 4), dtype=uint8)
        cl_data[ir(0.5*(bw-a)):ir(0.5*(bw+a)),ri:bw,0:3] = (255*sv).astype(uint8)
        cl_data[:,:,3] = self.cl_mask

        cl_img = PIL.Image.frombuffer('RGBA', (bw,bw), cl_data, 'raw', 'RGBA', 0, 1)
        
        # Rotate c-l-triangle
        cl_img = cl_img.rotate(self.hue*360)

        if (self.cl_img==None):
            self.cl_img = PIL.ImageTk.PhotoImage( cl_img )
            self.canvas.create_image( int(0.5*(W-bw)), int(0.5*(H-bw)), anchor=tk.NW, image=self.cl_img, tag='cl_img' )
            self.canvas.tag_bind('cl_img', '<Button-1>', self.foo_cl)
        else:
            self.cl_img.paste(cl_img)

        

    def foo_cl(self, event):
        print 'Fooo cl'
                           
        
    def on_hue_click(self, event):
        x = clip( event.x, 0, self.hue_panel_size[0] )
        y = clip( event.y, 0, self.hue_panel_size[1] )
        print 'x,y =', x, y
        xm,ym = self.hue_panel_size[0]/2., self.hue_panel_size[1]/2.
        r = sqrt( (x-xm)**2 + (y-ym)**2 )
        if r < self.r0: return
        phi = -arctan2(y-ym, x-xm)
        self.hue = phi/(2*pi)
        if self.hue<0: self.hue += 1
        if self.hue==-0.0: self.hue = 0.0
        print "hue=", self.hue
        self.draw()
        self.broadcast_color()

    def on_tab_changed(self, event):
        print 'H(CL) tab'
        self.draw()
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
        
