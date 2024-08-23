#include <iostream>
#include <vector>
#include "lodepng.h"

using namespace std;

// Checks whether the bit is set or not at a particular position.
// Returns true if set
// Returns false if not set
bool isBitSet(unsigned char byte, int pos) {
    byte = byte >> pos;
    return (byte & 1);
}

int main(int argc, char** argv) {

    // Check if the correct number of arguments are passed
    if(argc != 2) {
        cout << "Arguments Error: Provide the path to the encoded image.\n";
        return -1;
    }

    // Load the image using lodepng
    vector<unsigned char> image; // the raw pixels
    unsigned width, height;

    unsigned error = lodepng::decode(image, width, height, argv[1]);

    // If there's an error, display it
    if(error) {
        cout << "Image Error: " << lodepng_error_text(error) << "\n";
        return -1;
    }

    // Initialize variables to decode the message
    char ch = 0;
    int bit_count = 0;

    // Iterate through the image to extract the hidden message
    for(size_t i = 0; i < image.size(); i += 4) { // PNG images are RGBA (4 bytes per pixel)
        for(int color = 0; color < 3; color++) { // Only decode from RGB channels, ignore alpha

            // Check if the least significant bit is set
            if(isBitSet(image[i + color], 0))
                ch |= 1;

            bit_count++;

            // If we have read 8 bits, we have one character
            if(bit_count == 8) {

                // Check for the null character, which indicates the end of the message
                if(ch == '\0')
                    goto OUT;

                cout << ch;  // Print the character

                // Reset for the next character
                bit_count = 0;
                ch = 0;
            }
            else {
                // Shift left to make space for the next bit
                ch = ch << 1;
            }
        }
    }

OUT:;
    cout << "\nMessage decoding completed successfully.\n";

    return 0;
}
