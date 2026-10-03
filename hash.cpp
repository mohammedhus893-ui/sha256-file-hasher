#include <iostream>
#include <string>
#include <fstream>
#include <openssl/sha.h>

/**
 * Computes the SHA-256 hash of the given file and prints it
 * to stdout as a 64-character hexadecimal string.
 *
 * The file is read and hashed in fixed-size chunks rather than
 * loaded into memory all at once, so this works efficiently
 * even on files larger than available RAM.
 *
 * @param filename Path to the file to hash.
 */
void hash_core(std::string& filename) {
    // Open the file in binary mode to avoid any OS-level
    // text transformations (e.g. line-ending conversion)
    // that could change the hash result.
    std::ifstream file(filename, std::ios::binary);

    // SHA256_CTX holds the internal state of the hashing
    // algorithm as data is fed into it incrementally.
    SHA256_CTX sha;
    SHA256_Init(&sha);

    // Buffer used to read the file in fixed-size chunks.
    char buffer[4096];

    // Read the file chunk by chunk and feed each chunk into
    // the running hash. The loop stops once a read returns
    // fewer bytes than the buffer size (end of file).
    while (file.read(buffer, sizeof(buffer))) {
        SHA256_Update(&sha, buffer, file.gcount());
    }

    // The final, smaller-than-buffer chunk at the end of the
    // file causes the while condition above to fail before it
    // gets hashed, so it must be processed separately here to
    // ensure every byte of the file is included in the hash.
    if (file.gcount() > 0) {
        SHA256_Update(&sha, buffer, file.gcount());
    }

    // Raw 32-byte (256-bit) binary digest produced by SHA-256.
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &sha);

    // Print each byte of the digest as a two-digit hexadecimal
    // number, producing the familiar 64-character hash string.
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
}

int main(int argc, char* argv[]) {
    // argc counts the arguments typed on the command line,
    // including the program name itself. If the user did not
    // supply a file path (argc < 2), show usage instructions
    // instead of continuing.
    if (argc < 2) {
        std::cerr << "الاستخدام: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    // argv[1] is the first argument after the program name —
    // in this case, the path to the file the user wants hashed.
    std::string filename = argv[1];

    hash_core(filename);
    return 0;
}