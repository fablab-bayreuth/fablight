
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

