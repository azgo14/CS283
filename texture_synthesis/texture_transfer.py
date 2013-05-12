from texture import Texture
from texture_patch import TexturePatch
from argparse import ArgumentParser, ArgumentTypeError
import matplotlib.pyplot as plt

def texture_transfer(args):
    print "Texture Transfer Starting..."
    source_texture = Texture()
    source_texture.load_texture(args.source_texture)
    target_image = Texture()
    target_image.load_texture(args.target_image)
    
    source_corr_texture = Texture()
    target_corr_texture = Texture()
    if args.corr_map_type == 0:
        source_corr_texture.load_texture(args.source_texture, gray=True)
        target_corr_texture.load_texture(args.target_image, gray=True)
    elif args.corr_map_type == 1:
        source_corr_texture.load_texture(args.source_texture,
                                         gray=True, blur=True)
        
        target_corr_texture.load_texture(args.target_image,
                                         gray=True, blur=True)
    patch_width = args.patch_width
    patch_height = args.patch_height

    new_t_height, new_t_width, dim = target_image.pixels.shape

    texture = Texture()
    texture.init_empty_texture(new_t_height, new_t_width, dim) 
    for step in range(args.iterations):
        if args.iterations == 1:
            alpha = .1
        else:
            alpha = .8 * step / (args.iterations-1) + .1
        patches, source_patch_cmap = \
            source_texture.create_patches(patch_height,
                                          patch_width,
                                          overlap=args.overlap_percent,
                                          cmap=source_corr_texture)
    
        texture = \
            Texture.create_from_source_transfer_target(patches,
                                                       source_patch_cmap,
                                                       target_corr_texture,
                                                       texture,
                                                       alpha)

        patch_width = int(patch_width * (1-args.patch_len_reduction))
        patch_height = int(patch_height * (1-args.patch_len_reduction))
        if patch_width == 0 or patch_height == 0:
            print "patch width or height reduced to 0, stopping iterations"
            break 
    texture.save_texture(args.output_file)
    
    return 

def parseType(string):
    try:
        parse_type = int(string)
    except ValueError:
        raise ArgumentTypeError("'" + string + "' is not an integer")
    if parse_type < 0 or parse_type > 1:
        raise ArgumentTypeError("'" + string + "' is not in the range 0-1")
    return parse_type

def parseDimension(string):
    try:
        parse_type = int(string)
    except ValueError:
        raise ArgumentTypeError("'" + string + "' is not an integer")
    if parse_type <= 0:
        raise ArgumentTypeError("'" + string + \
            "' needs to be a positive number")
    return parse_type

def parsePercent(string):
    try:
        parse_type = float(string)
    except ValueError:
        raise ArgumentTypeError("'" + string + "' is not an float")
    if parse_type < 0 or parse_type > .5:
        raise ArgumentTypeError("'" + string + "' is not in the range 0-.5")
    return parse_type

def process_flags():
    parser = ArgumentParser()
    parser.add_argument("source_texture",
                        help="Enter the path to the source texture file")

    parser.add_argument("target_image",
                        help="Enter the path to the target image file")

    parser.add_argument("corr_map_type", type=parseType,
                        help="Input the correspondence map to use: " + \
                        "0 - Intensity, 1 - Blurred Intensity") 
    
    parser.add_argument("iterations", type=parseDimension,
                        help="Enter the number of iterations to use")

    parser.add_argument("-f", "--output_file",
                        help="The ouput filename for the synthesized " + \
                        "texture. Default is 'synthesized_texture.png'",
                        default="synthesized_texture.png")
 
    parser.add_argument('-pw', "--patch_width", default=50,
                        type=parseDimension,
                        help="The starting patch width used. [DEFAULT=50]")

    parser.add_argument('-ph', "--patch_height", default=50,
                        type=parseDimension,
                        help="The starting patch height used. [DEFAULT=50]")

    parser.add_argument('-o', '--overlap_percent', default=.166,
                        type=parsePercent,
                        help="The percentage overlap to use. " + \
                        "[DEFAULT=.166].")

    parser.add_argument('-r', '--patch_len_reduction', type=parsePercent,
                        default='.3333',
                        help="The percentage decrease in patch length per " + \
                        "iteration. [DEFAULT=.333].")

    return parser.parse_args()

if __name__ == "__main__":
    args = process_flags()
    texture_transfer(args)
