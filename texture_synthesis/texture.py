from texture_patch import TexturePatch
import matplotlib.pyplot as plt
import matplotlib.image as mplimg
import scipy.misc
import numpy as np
from random import choice
import math

class Texture:
    # self.pixels is a 3D numpy array (last dimension is the RGB values)
    def __init__(self):
        self.pixels = np.array([])

    def __getitem__(self, key):
        return self.pixels[key]

    def __setitem__(self, key, val):
        self.pixels[key] = val

    def init_empty_texture(self, height, width, dim):
        self.pixels = np.zeros((height, width, dim)) 

    def load_texture(self, filename):
        print "Loading texture from " + filename
        self.pixels = mplimg.imread(filename)
        print "Done loading texture"

    def save_texture(self, filename):
        print self.pixels.shape
        scipy.misc.imsave(filename, self.pixels)

    def create_patches(self, p_height, p_width, overlap=0):
        print "Creating patches..."
        height_overlap = int(math.floor(p_height * overlap))
        width_overlap = int(math.floor(p_width * overlap))
        p_height += height_overlap
        p_width += width_overlap
        t_height,t_width,dim = self.pixels.shape
        patches = []
        if t_width < p_width and t_height < p_height:
            print "Given patch width or height dimensions " + \
                  "exceed texture dimension" 
            exit(1)
        for row in range(t_height - p_height + 1):
            for column in range(t_width - p_width + 1):
                if row + p_height > t_height or column + p_width > t_width:
                    print "DELETE 1"
                    exit(1)
                patches.append(
                    TexturePatch(self[row:row+p_height,
                                      column:column+p_width,:],
                                 height_overlap, width_overlap))
        print "Done creating patches"
        return patches

    @staticmethod
    def create_simple_tex_from_patches(patches, new_t_height, new_t_width):
        if len(patches) <= 0:
            print "You did not give me any patches"
            exit(1)   
        print "Creating simple texture from patches..."
        p_width,p_height,dim = patches[0].pixels.shape
         
        w_patch_start_indices = range(new_t_width)[0::p_width]
        h_patch_start_indices = range(new_t_height)[0::p_height]

        new_texture = Texture()
        new_texture.init_empty_texture(new_t_height, new_t_width, dim)

        for h_start in h_patch_start_indices:
            for w_start in w_patch_start_indices:
                h_end = h_start + p_height
                w_end = w_start + p_width
               
                if h_end > new_t_height:
                    h_end = new_t_height 
                if w_end > new_t_width:
                    w_end = new_t_width
                
                random_patch = choice(patches)
                new_texture[h_start:h_end,w_start:w_end,:] = \
                    random_patch[0:h_end-h_start,0:w_end-w_start,:]                
        print "Done creating simple texture from patches."
        return new_texture

    @staticmethod
    def create_overlap_tex_from_patches(patches, new_t_height,
                                        new_t_width, overlap):
        if len(patches) <= 0:
            print "You did not give me any patches"
            exit(1)   
        print "Creating overlap texture from patches..."
        p_width,p_height,dim = patches[0].pixels.shape
         
        w_patch_start_indices = range(new_t_width)[0::p_width]
        h_patch_start_indices = range(new_t_height)[0::p_height]

        new_texture = Texture()
        new_texture.init_empty_texture(new_t_height, new_t_width, dim)

         

    @staticmethod
    def create_mincut_tex_from_patches(patches, new_t_height,
                                       new_t_width, overlap):
        if len(patches) <= 0:
            print "You did not give me any patches"
            exit(1)   
        print "Creating simple texture from patches..."
        p_width,p_height,dim = patches[0].pixels.shape
         
        w_patch_start_indices = range(new_t_width)[0::p_width]
        h_patch_start_indices = range(new_t_height)[0::p_height]

        new_texture = Texture()
        new_texture.init_empty_texture(new_t_height, new_t_width, dim)
        pass
