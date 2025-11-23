#include "sha256_wrapper.h" // The C header
#include "SHA_Hash.h"       // Your C++ SHA-256 implementation header

#include <string>
#include <vector>
#include <cstring> // For strncpy

// 'extern "C"' prevents C++ name mangling, so C code can find the function.
extern "C" {
    void hash_password_c(const char* password_input, char* hex_hash_output, int output_buffer_size) {
        
        // --- This block of code is the logic from your old main() function ---
        
        // 1. Convert C-style input string to C++ std::string
        std::string message(password_input);

        // 2. Preprocess the message
        std::vector<uint8_t> padded = preprocess(message);

        // 3. Initialize hash values and constants
        uint32_t H[8], K[64];
        initialize(H, K);

        // 4. Process the blocks
        processBlocks(padded, H, K);

        // 5. Get the final hash as a hex string
        std::string hash_hex_str = digestToHex(H);
        
        // --- End of hashing logic ---

        // 6. Safely copy the resulting C++ string to the C output buffer
        if (hex_hash_output != nullptr && output_buffer_size > 0) {
            strncpy(hex_hash_output, hash_hex_str.c_str(), output_buffer_size - 1);
            // Ensure null-termination
            hex_hash_output[output_buffer_size - 1] = '\0';
        }
    }
}