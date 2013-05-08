from texture_patch import TexturePatch
import matplotlib.pyplot as plt
import matplotlib.image as mplimg
import numpy as np
from random import choice


class Texture:
    # self.pixels is a 2D numpy array
    def __init__(self, height, width):
        self.pixels = np.zeros((height, width)) 

    def load_texture(self, filename):
        print "Loading texture from " + filename
        self.pixels = mplimg.imread(filename)
        print "Done loading texture"

    def save_texture(self, filename):
        mplimg.imsave(filename, self.pixels)

    def create_patches(self, p_height, p_width):
        print "Creating patches..."
        t_height,t_width = self.pixels.shape
        patches = []
        if t_width < p_width and t_height < p_height:
            print "Given patch width or height dimensions " + \
                  "exceed texture dimension" 
            exit(1)
        for row in range(t_height - p_height + 1):
            for column in range(t_width - p_width + 1):
                if row + p_height >= t_height or column + p_width >= t_width:
                    print "DELETE 1"
                    exit(1)
                patches.append(
                    TexturePatch(self.patches[row:row+p_height,
                                              column:column+p_width]))
        return patches

    @staticmethod
    def create_simple_tex_from_patches(patches, new_t_height, new_t_width):
        if len(patches) <= 0:
            print "You did not give me any patches"
            exit(1)   
        p_width,p_height = patches[0].shape
         
        w_patch_start_indices = range(new_t_width)[0::p_width]
        h_patch_start_indices = range(new_t_height)[0::p_height]

        new_texture = Texture(new_t_height, new_t_width)

        for h_start in h_patch_start_indices:
            for w_start in w_patch_start_indices:
                h_end = h_start + p_height
                w_end = h_width + p_width
               
                if h_end > new_t_height:
                    h_end = new_t_height 
                if w_end > new_t_width:
                    w_end = new_t_width
                
                random_patch = choice(patches)
                
                new_texture.pixels[h_start:h_end,w_start:w_end] = \
                    random_patch[0:h_end-h_start,0:w_end-w_start]                
        
        return new_texture


    
