import numpy as np
from operator import itemgetter
import matplotlib.pyplot as plt

def compute_l2_error(nparray1, nparray2):
    if nparray1.shape != nparray2.shape:
        print "2 given numpy arrays don't have the same shape"
        exit(1)
    return np.sum((np.subtract(nparray1,nparray2))**2) 

# Assumes that mincut is always top to bottom
def compute_mincut(nparray1, nparray2):
    if len(nparray1.shape) == 3:
        height,width,dim = nparray1.shape
    elif len(nparray2.shape) == 2:
        height,width = nparray1.shape
        dim = 1
    print nparray1.shape
    error_matrix = np.zeros([height, width])
    path_pointer_matrix = np.zeros([height, width])
    for row_index in range(height):
        for col_index in range(width):
            prev_lowest = float('inf')
            prev_lowest_index = -1
            if row_index == 0:
                prev_lowest = 0
            else:
                choices = [-1, 0, 1]    
                for choice in choices:
                    current_c_index = col_index + choice
                    if current_c_index < 0 or current_c_index >= width:
                        continue
                    error = error_matrix[row_index-1, current_c_index]
                    if error < prev_lowest:
                        prev_lowest = error
                        prev_lowest_index = current_c_index
            current_error = np.sum((nparray1[row_index,col_index,:] - \
                                    nparray2[row_index,col_index,:])**2)
            error_matrix[row_index,col_index] = prev_lowest + current_error
            path_pointer_matrix[row_index,col_index] = prev_lowest_index

    mincut_path = []
    min_index,val = min(enumerate(error_matrix[row_index,:]),
                        key=itemgetter(1))                        
    mincut_path.append((height - 1,min_index))
    next_index = path_pointer_matrix[height - 1, min_index]

    for row_index in reversed(range(height - 1)):
        mincut_path.append((row_index,next_index))
        next_index = path_pointer_matrix[row_index, next_index]

    return mincut_path[::-1] # reverses the list

class TexturePatch:
    # self.pixels is a 2D numpy array 
    def __init__(self, patches, height_overlap, width_overlap):
        if len(patches.shape) == 3:
            combined_h,combined_w,dim = patches.shape
        elif len(patches.shape) == 2:
            combined_h,combined_w = patches.shape
            dim = 1
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
    def get_l2_error(self, other_patch, left=True):
        if left:
            return compute_l2_error(self.right_overlap,
                                    other_patch.left_overlap)

        return compute_l2_error(self.bottom_overlap,
                                other_patch.top_overlap) 
            
    def __get_mincut_boundary(self, other_patch, left=True):
        if left:
            return compute_mincut(self.right_overlap,
                                  other_patch.left_overlap)
        
        mincut_path = compute_mincut(np.transpose(self.bottom_overlap,
                                     axes=(1,0,2)),
                                     np.transpose(other_patch.top_overlap,
                                     axes=(1,0,2)))
        new_path = []
        for step in mincut_path:
            r,c = step
            new_path.append((c,r))

        return new_path                       

    def modify_by_mincut_boundary(self, other_patch, left=True):
        mincut_path = self.__get_mincut_boundary(other_patch, left)
        if left:
            if len(self.right_overlap.shape) == 3:
                height,width,dim = self.right_overlap.shape
            elif len(self.right_overlap.shape) == 2:
                height,width = self.right_overlap.shape
                dim = 1
            mask_matrix = np.zeros([height, width, dim], dtype=np.int)
            mincut_boundary = np.zeros([height, width, dim], dtype=np.int)
            for step in mincut_path:
                row,col = step
                mask_matrix[row,col:,:] = 1 # 0 for current, 1 for other       
            mincut_boundary[mask_matrix == 0] = \
                self.right_overlap[mask_matrix == 0]

            mincut_boundary[mask_matrix == 1] = \
                other_patch.left_overlap[mask_matrix == 1]
           
            if len(self.pixels.shape) == 3: 
                p_height,p_width,dim = self.pixels.shape
            elif len(self.pixels.shape) == 2:
                p_height,p_width = self.pixels.shape
                dim = 1
            p_height_overlay = (height - p_height) / 2
            p_width_overlay = width / 2

            self.right_overlap[:,:] = mincut_boundary[:,:]
            
            other_patch.left_overlap[:,:] = mincut_boundary[:,:]
        else:
            if len(self.bottom_overlap.shape) == 3:
                height,width,dim = self.bottom_overlap.shape
            elif len(self.bottom_overlap.shape) == 2:
                height,width = self.bottom_overlap.shape
                dim = 1
            mmask_matrix = np.zeros([height, width, dim])
            mincut_boundary = np.zeros([height, width, dim])
            for step in mincut_path:
                row,col = step
                mask_matrix[row:,col,:] = 1 # 0 for current, 1 for other       
            print mincut_path
            mincut_boundary[mask_matrix == 0] = \
                self.bottom_overlap[mask_matrix == 0]

            mincut_boundary[mask_matrix == 1] = \
                other_patch.top_overlap[mask_matrix == 1]

            if len(self.pixels.shape) == 3: 
                p_height,p_width,dim = self.pixels.shape
            elif len(self.pixels.shape) == 2:
                p_height,p_width = self.pixels.shape
                dim = 1
            p_height_overlay = height / 2
            p_width_overlay = (width - p_width) / 2

            self.bottom_overlap[:,:] = mincut_boundary[:,:]
 
            other_patch.top_overlap[:,:] = mincut_boundary[:,:]
                        


