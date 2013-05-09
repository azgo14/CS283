import numpy as np

def computeL2Error(nparray1, nparray2):
    if nparray1.shape != nparray2.shape:
        print "2 given numpy arrays don't have the same shape"
        exit(1)
    return sum(sum((nparray1 - nparray2)**2))  

class TexturePatch:
    # self.pixels is a 2D numpy array 
    def __init__(self, patches, height_overlap, width_overlap):
        combined_h,combined_w,dim = patches.shape
        p_height = combined_h - 2*height_overlap
        p_width = combined_w - 2*width_overlap
    
        # See paper for what the overlap looks like and why we did it this way
        self.left_overlap = patches[:,0:2*width_overlap]
        self.top_overlap = patches[0:2*height_overlap,:]
        self.bottom_overlap = patches[p_height:,:]
        self.right_overlap = patches[:, p_width:]

        self.pixels = patches[height_overlap:height_overlap+p_height,
                              width_overlap:width_overlap+p_width]
     
    def __getitem__(self, key):
        return self.pixels[key]

    def __setitem__(self, key, val):
        self.pixels[key] = val

    # Assume self patch is either left or top of other_patch
    def getL2Error(self, other_patch, left=True):
        if left:
            return computeL2Error(self.right_overlap,
                                  other_patch.left_overlap)

        return computeL2Error(self.bottom_overlap,
                              other_path.top_overlap) 
            
    # Returns the indices of the minimum boundary path
    def get_minimum_error(self, another_block):
        pass            
