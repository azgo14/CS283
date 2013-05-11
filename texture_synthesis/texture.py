from texture_patch import TexturePatch
import matplotlib.pyplot as plt
import matplotlib.image as mplimg
import scipy.misc
import numpy as np
from random import choice,randint,random
import math
from operator import itemgetter

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
        self.pixels = np.atleast_3d(scipy.misc.imread(filename))
        print "Done loading texture"

    def save_texture(self, filename):
        scipy.misc.imsave(filename, np.squeeze(self.pixels))

    def create_patches(self, p_height, p_width, overlap=0):
        print "Creating patches..."
        height_overlap = int(math.floor(p_height * overlap))
        width_overlap = int(math.floor(p_width * overlap))
        p_height += 2 * height_overlap #overlap on both sides
        p_width += 2 * width_overlap
        if len(self.pixels.shape) == 3:
            t_height,t_width,dim = self.pixels.shape
        elif len(self.pixels.shape) == 2:
            t_height,t_width = self.pixels.shape
            dim = 1
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
                if random() > .1:
                    continue
                patches.append(
                    TexturePatch(np.copy(self[row:row+p_height,
                                         column:column+p_width]),
                                 height_overlap, width_overlap))
        print "Done creating patches"
        return patches

    @staticmethod
    def create_simple_tex_from_patches(patches, new_t_height, new_t_width):
        if len(patches) <= 0:
            print "You did not give me any patches"
            exit(1)   
        print "Creating simple texture from patches..."
        if len(patches[0].pixels.shape) == 3:
            p_height,p_width,dim = patches[0].pixels.shape
        elif len(patches[0].pixels.shape) == 2:
            p_height,p_width = patches[0].pixels.shape
            dim = 1 
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
    def __find_minimum_L2_patch(patches, left_patch_index, top_patch_index):
        l2_error_list = [0] * len(patches)
        print len(patches)        
        if left_patch_index != -1:
            left_patch = patches[left_patch_index]
            for index,patch in enumerate(patches):
                l2_error_list[index] += left_patch.get_l2_error(patch,
                                                                left=True) 
        if top_patch_index != -1:
            top_patch = patches[top_patch_index]
            for index,patch in enumerate(patches):
                l2_error_list[index] += top_patch.get_l2_error(patch,
                                                               left=False)
        
        value = min(enumerate(l2_error_list), key=itemgetter(1))[1]
        threshold = value * 1.01
        threshold_list = [error <= threshold for error in l2_error_list]
        choice_list = np.nonzero(threshold_list)[0].tolist()
        print len(choice_list)
        return choice(choice_list)

    @staticmethod
    def create_overlap_tex_from_patches(patches, new_t_height,
                                        new_t_width):
        if len(patches) <= 0:
            print "You did not give me any patches"
            exit(1)   
        print "Creating overlap texture from patches..."
        
        if len(patches[0].pixels.shape) == 3:
            p_height,p_width,dim = patches[0].pixels.shape
        elif len(patches[0].pixels.shape) == 2:
            p_height,p_width = patches[0].pixels.shape
            dim = 1 
         
        w_patch_start_indices = range(new_t_width)[0::p_width]
        h_patch_start_indices = range(new_t_height)[0::p_height]

        new_texture = Texture()
        new_texture.init_empty_texture(new_t_height, new_t_width, dim)

        patch_index = -np.ones((new_t_height, new_t_width), dtype=np.int)

        for h_start in h_patch_start_indices:
            for w_start in w_patch_start_indices:
                h_end = h_start + p_height
                w_end = w_start + p_width
               
                if h_end > new_t_height:
                    h_end = new_t_height 
                if w_end > new_t_width:
                    w_end = new_t_width

                chosen_patch = None 
                chosen_index = -1
                left_index = patch_index[h_start, w_start-1]
                top_index = patch_index[h_start - 1, w_start]
                if left_index == -1 and top_index == -1:
                    chosen_index = randint(0, len(patches) - 1)
                else:
                    chosen_index = Texture.__find_minimum_L2_patch(patches,
                                                                   left_index,
                                                                   top_index)  
                chosen_patch = patches[chosen_index]
                new_texture[h_start:h_end,w_start:w_end,:] = \
                    chosen_patch[0:h_end-h_start,0:w_end-w_start,:]                
               
                patch_index[h_start:h_end, w_start:w_end] = \
                    chosen_index * np.ones((h_end-h_start,w_end-w_start),
                                           dtype=np.int)
        return new_texture

    @staticmethod
    def __calculate_mincut(current_patch, right_patch, bottom_patch):
        if right_patch is not None:
            current_patch.modify_by_mincut_boundary(right_patch)    
                        
        if bottom_patch is not None:
            current_patch.modify_by_mincut_boundary(bottom_patch,
                                                    left=False)
                     
    @staticmethod
    def create_mincut_tex_from_patches(patches, new_t_height,
                                       new_t_width):
        if len(patches) <= 0:
            print "You did not give me any patches"
            exit(1)   
        print "Creating mincut overlap texture from patches..."
        
        if len(patches[0].pixels.shape) == 3:
            p_height,p_width,dim = patches[0].pixels.shape
        elif len(patches[0].pixels.shape) == 2:
            p_height,p_width = patches[0].pixels.shape
            dim = 1 
        
        w_patch_start_indices = range(new_t_width)[0::p_width]
        h_patch_start_indices = range(new_t_height)[0::p_height]

        patch_index = -np.ones((new_t_height, new_t_width), dtype=np.int)
        unique_patch_list = []

        # pick all the patches
        for h_start in h_patch_start_indices:
            for w_start in w_patch_start_indices:
                h_end = h_start + p_height
                w_end = w_start + p_width
               
                if h_end > new_t_height:
                    h_end = new_t_height 
                if w_end > new_t_width:
                    w_end = new_t_width

                chosen_patch = None 
                chosen_index = -1
                left_index = patch_index[h_start, w_start-1]
                top_index = patch_index[h_start - 1, w_start]
                if left_index == -1 and top_index == -1:
                    chosen_index = randint(0, len(patches) - 1)
                else:
                    chosen_index = Texture.__find_minimum_L2_patch(patches,
                                                                   left_index,
                                                                   top_index)  
                
                chosen_patch = TexturePatch.copy_patch(patches[chosen_index])
                chosen_index = len(unique_patch_list)
                unique_patch_list.append(chosen_patch)

                patch_index[h_start:h_end, w_start:w_end] = \
                    chosen_index * np.ones((h_end-h_start,w_end-w_start),
                                           dtype=np.int)
        
        
        new_texture = Texture()
        new_texture.init_empty_texture(new_t_height, new_t_width, dim)

        # compute the mincuts
        for h_start in h_patch_start_indices:
            for w_start in w_patch_start_indices:
                h_end = h_start + p_height
                w_end = w_start + p_width
               
                current_patch = unique_patch_list[patch_index[h_start,w_start]]
                right_patch = None
                bottom_patch = None
                
                if h_end >= new_t_height:
                    h_end = new_t_height
                else:
                    bottom_patch = \
                        unique_patch_list[patch_index[h_end, w_start]] 
                if w_end >= new_t_width:
                    w_end = new_t_width
                else:
                    right_patch = \
                        unique_patch_list[patch_index[h_start, w_end]]
                
                Texture.__calculate_mincut(current_patch,
                                           right_patch, bottom_patch) 

                new_texture[h_start:h_end,w_start:w_end,:] = \
                    current_patch[0:h_end-h_start,0:w_end-w_start,:]                
                  
        return new_texture
  
