from texture import Texture
from texture_patch import TexturePatch
from argparse import ArgumentParser, ArgumentTypeError

def simple_patches_synthesizer(args):
    print "Using Simple Patches Synthesizer"
    default_texture = Texture()
    default_texture.load_texture(args.input_file)
    patches = default_texture.create_patches(args.patch_height,
                                             args.patch_width)
    new_texture = Texture.create_simple_tex_from_patches(patches,
                                                         args.height,
                                                         args.width)   
    new_texture.save_texture(args.output_file)
    return

def overlap_patches_synthesizer(args):
    print "Using Overlap Patches Synthesizer"
    default_texture = Texture()
    default_texture.load_texture(args.input_file)
    patches = default_texture.create_patches(args.patch_height,
                                             args.patch_width,
                                             overlap=args.overlap_percentage)
    new_texture = Texture.create_overlap_tex_from_patches(patches,
                                                          args.height,
                                                          args.width)
    new_texture.save_texture(args.output_file)
    return


def mincut_overlap_patches_synthesizer(args):
    print "Using Mincut Overlap Patches Synthesizer"
    default_texture = Texture()
    default_texture.load_texture(args.input_file)
    patches = default_texture.create_patches(args.patch_height,
                                             args.patch_width,
                                             overlap=args.overlap_percentage)
    new_texture = Texture.create_mincut_tex_from_patches(patches,
                                                         args.height,
                                                         args.width)
    new_texture.save_texture(args.output_file)
    return 

def parseType(string):
    try:
        parse_type = int(string)
    except ValueError:
        raise ArgumentTypeError("'" + string + "' is not an integer")
    if parse_type < 0 or parse_type > 2:
        raise ArgumentTypeError("'" + string + "' is not in the range 0-2")
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
    parser.add_argument("type", type=parseType,
                        help="Input the synthesizer algorithm to use: " + \
                        "0 - Simple Patches, 1 - Overlap, 2 - Minimum Cut") 
    
    parser.add_argument("input_file",
                        help="Enter the path to the base texture file")
    
    parser.add_argument("-f", "--output_file",
                        help="The ouput filename for the synthesized " + \
                        "texture. Default is 'synthesized_texture.png'",
                        default="synthesized_texture.png")
 
    parser.add_argument("width", type=parseDimension,
                        help="Input the width of the output texture")

    parser.add_argument("height", type=parseDimension,
                        help="Input the height of the output texture")

    parser.add_argument('-pw', "--patch_width", default=50,
                        type=parseDimension,
                        help="The patch width used. [DEFAULT=50]")

    parser.add_argument('-ph', "--patch_height", default=50,
                        type=parseDimension,
                        help="The patch height used. [DEFAULT=50]")

    parser.add_argument('-o', '--overlap_percent', default=.166,
                        type=parsePercent,
                        help="The percentage overlap to use. " + \
                        "[DEFAULT=.166]. Only affects type 1 and 2")

    return parser.parse_args()

if __name__ == "__main__":
    args = process_flags()
    if args.type == 0:
        simple_patches_synthesizer(args) 
    elif args.type == 1:
        overlap_patches_synthesizer(args)
    else:
        mincut_overlap_patches_synthesizer(args)
