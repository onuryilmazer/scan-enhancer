//Before using the STB library, we need to call the macros that instantiate the function implementations.
//This needs to be done exactly once, so we do it in a separate file here, and add this to our CMAKE target.

//stb_image.h:
#define STB_IMAGE_IMPLEMENTATION

//stb_image_write.h:
#define STB_IMAGE_WRITE_IMPLEMENTATION