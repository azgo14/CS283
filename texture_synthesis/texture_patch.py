import numpy as np

class TexturePatch:
    # self.pixels is a 2D numpy array 
    def __init__(self, patches, height_overlap, width_overlap):
        combined_h,combined_w,dim = patches.shape
        p_height = combined_h - 2*height_overlap
        p_width = combined_w - 2*width_overlap
    
        self.left_overlap = patches[:,0:width_overlap]
        self.top_overlap = patches[0:height_overlap,:]
        self.bottom_overlap = patches[height_overlap+p_height:,:]
        self.right_overlap = patches[:, width_overlap+p_width:]
        self.pixels = patches[height_overlap:height_overlap+p_height,
                              width_overlap:width_overlap+p_width]
     
    def __getitem__(self, key):
        return self.pixels[key]

    def __setitem__(self, key, val):
        self.pixels[key] = val

    #assume this patch is on the left/top

    # Returns the indices of the minimum boundary path
    def get_minimum_error(self, another_block):
        pass            
