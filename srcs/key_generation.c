#include "woody.h"

bool generate_key(unsigned char *key, size_t size) {
    int urandom = open("/dev/urandom", O_RDONLY);
    if (urandom < 0) {
        perror("open");
        return false;
    }

    ssize_t result = read(urandom, key, size);
    if (result < 0) {
        perror("read");
        close(urandom);
        return false;
    }

    close(urandom);
    return true;
}

// int main() {
//     unsigned char key[key_size];
//     generate_key(key, key_size);

//     printf("Generated key: ");
//     for (int i = 0; i < key_size; i++) {
//         printf("%02x", key[i]);
//     }
//     printf("\n");

//     return 0;
// }