#ifndef SHA256_WRAPPER_H
#define SHA256_WRAPPER_H

// This is the C-callable function that will be implemented in C++.
void hash_password_c(const char* password_input, char* hex_hash_output, int output_buffer_size);

#endif // SHA256_WRAPPER_H