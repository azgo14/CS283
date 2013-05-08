import numpy as np

class TexturePatch:
    # self.pixels is a 2D numpy array 
    def __init__(self, patches):
        self.pixels = patches
    
    def __getitem__(self, key):
        return self.pixels[key]

    def __setitem__(self, key, val):
        self.pixels[key] = val

    # Returns the indices of the minimum boundary path
    def get_minimum_error(self, another_block):
        pass            
