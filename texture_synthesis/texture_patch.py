import numpy as np

class TexturePatch:
    # self.pixels is a 2D numpy array 
    def __init__(self, patches):
        self.pixels = patches

    # Returns the indices of the minimum boundary path
    def get_minimum_error(self, another_block):
        pass            
