from texture import Texture
from texture_patch import TexturePatch
from argparse import ArgumentParser, ArgumentTypeError

def simple_patches_synthesizer():
    print "Using Simple Patches Synthesizer"
    pass

def overlap_patches_synthesizer():
    print "Using Overlap Patches Synthesizer"
    pass

def mincut_overlap_patches_synthesizer():
    print "Using Mincut Overlap Patches Synthesizer"
    pass 

def parseType(string):
    try:
        parse_type = int(string)
    except ValueError:
        raise ArgumentTypeError("'" + string + "' is not an integer")
    if parse_type < 0 or parse_type > 2:
        raise ArgumentTypeError("'" + string + "' is not in the range 0-2")
    return parse_type

def process_flags():
    parser = ArgumentParser()
    parser.add_argument("type", type=parseType,
                        help="Input the synthesizer algorithm to use: " + \
                        "0 - Simple Patches, 1 - Overlap, 2 - Minimum Cut") 
    

    return parser.parse_args()

if __name__ == "__main__":
    args = process_flags()
   
    if args.type == 0:
        simple_patches_synthesizer() 
    elif args.type == 1:
        overlap_patches_synthesizer()
    else:
        mincut_overlap_patches_synthesizer()
