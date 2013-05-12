from texture import Texture
from texture_patch import TexturePatch
from argparse import ArgumentParser, ArgumentTypeError

def texture_transfer(args):
    print "Using Mincut Overlap Patches Synthesizer"
    default_texture = Texture()
    default_texture.load_texture(args.input_file)
    patches = default_texture.create_patches(args.patch_height,
                                             args.patch_width,
                                             overlap=args.overlap_percent)
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

    parser.add_argument("corr_map", type=parseType,
                        help="Input the correspondence map to use: " + \
                        "0 - Intensity, 1 - Blurred Intensity") 
    
    parser.add_argument("iterations", type=parseDimension,
                        help="Enter the number of iterations to use")

    parser.add_argument("-f", "--output_file",
                        help="The ouput filename for the synthesized " + \
                        "texture. Default is 'synthesized_texture.png'",
                        default="synthesized_texture.png")
 
    parser.add_argument('-pw', "--patch_width", default=100,
                        type=parseDimension,
                        help="The starting patch width used. [DEFAULT=100]")

    parser.add_argument('-ph', "--patch_height", default=100,
                        type=parseDimension,
                        help="The starting patch height used. [DEFAULT=100]")

    parser.add_argument('-o', '--overlap_percent', default=.166,
                        type=parsePercent,
                        help="The percentage overlap to use. " + \
                        "[DEFAULT=.166].")

    return parser.parse_args()

if __name__ == "__main__":
    args = process_flags()
    texture_transfer(args)
