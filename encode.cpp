#include <iostream>
#include <fstream>
#include <vector>
#include "lodepng.h"

using namespace std;

// Checks whether the bit is set or not at a particular position.
bool isBitSet(char ch, int pos) {
    ch = ch >> pos;
    return (ch & 1);
}

int main(int argc, char** argv) {

    if(argc != 4) {
        cout << "Arguments Error" << "\n";
        return -1;
    }

    // Load the image using lodepng
    vector<unsigned char> image; // the raw pixels
    unsigned width, height;

    unsigned error = lodepng::decode(image, width, height, argv[1]);

    if(error) {
        cout << "Image Error: " << lodepng_error_text(error) << "\n";
        return -1;
    }

    size_t max_message_length = (width * height * 3) / 8;
    cout << "Max message length: " << max_message_length << " characters.\n";

    // Open file for text information
    ifstream file(argv[2], ios::in | ios::binary | ios::ate);
    if(!file.is_open()) {
        cout << "File Error\n";
        return -1;
    }

    size_t message_length = file.tellg();
    file.seekg(0, ios::beg);
    cout << "Message length: " << message_length << " characters.\n";

    if(message_length > max_message_length) {
        cout << "Message too big. Try with a larger image.\n";
        return -1;
    }

    char ch;
    int bit_count = 0;
    bool last_null_char = false;
    bool encoded = false;

    file.get(ch);
    cout << "First character to encode: " << ch << "\n";

    for(size_t i = 0; i < image.size(); i += 4) { // PNG images are RGBA (4 bytes per pixel)
        for(int color = 0; color < 3; color++) { // Only encode in RGB channels, ignore alpha

            if(isBitSet(ch, 7-bit_count))
                image[i + color] |= 1; // Set LSB to 1
            else
                image[i + color] &= ~1; // Set LSB to 0

            bit_count++;

            if(last_null_char && bit_count == 8) {
                encoded = true;
                cout << "Encoding completed successfully.\n";
                goto OUT;
            }

            if(bit_count == 8) {
                bit_count = 0;
                if(file.get(ch)) {
                    cout << "Next character to encode: " << ch << "\n";
                } else {
                    last_null_char = true;
                    ch = '\0';
                    cout << "End of file reached, encoding null character.\n";
                }
            }
        }
    }

OUT:;

    if(!encoded) {
        cout << "Message too big. Try with a larger image.\n";
        return -1;
    }

    error = lodepng::encode(argv[3], image, width, height);

    if(error) {
        cout << "Encoding Error: " << lodepng_error_text(error) << "\n";
        return -1;
    }

    cout << "Message encoded successfully!\n";
    return 0;
}
