# SHA-256 File Hasher

A simple command-line tool written in C++ that computes the
SHA-256 cryptographic hash (fingerprint) of any file. This is
the core building block of a **file integrity checker** — a
defensive security tool that can later detect unauthorized
changes to important files by comparing hashes over time.

## What It Does

Given a file path, the program:
1. Opens the file in binary mode
2. Reads it in 4096-byte chunks
3. Feeds each chunk into the SHA-256 algorithm incrementally
4. Prints the final 64-character hexadecimal hash to the console

Because SHA-256 always produces a fixed-length 64-character
output regardless of input size, this hash can be used as a
compact, reliable "fingerprint" to verify a file's contents
have not changed.

## How It Works (Code Walkthrough)

```cpp
void hash_core(std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    SHA256_CTX sha;
    SHA256_Init(&sha);
    char buffer[4096];
    while (file.read(buffer, sizeof(buffer))) {
        SHA256_Update(&sha, buffer, file.gcount());
    }
    if (file.gcount() > 0) {
        SHA256_Update(&sha, buffer, file.gcount());
    }
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &sha);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
}
```

- **`SHA256_Init`** initializes a hashing context that will
  accumulate state as data is fed into it.
- **`SHA256_Update`** is called once per chunk read from the
  file, incrementally updating the hash state. This streaming
  approach means the tool can hash files far larger than
  available memory, since it never loads the whole file at once.
- The `if (file.gcount() > 0)` check after the loop ensures the
  final, smaller-than-4096-byte chunk at the end of the file
  (which causes the `while` condition to fail) still gets
  included in the hash calculation.
- **`SHA256_Final`** finalizes the computation and writes the
  raw 32-byte digest into the `hash` array.
- The closing loop prints each byte as a two-digit hexadecimal
  number, producing the familiar 64-character hash string.

```cpp
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "الاستخدام: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }
    std::string filename = argv[1];
    hash_core(filename);
    return 0;
}
```

- `argc`/`argv` let the program accept the target file name as
  a command-line argument instead of hardcoding it, so the tool
  can be reused on any file without editing the source code.
- A basic usage check prevents the program from running with no
  file specified.

## Requirements

- A C++ compiler (`g++`)
- OpenSSL development headers (`libssl-dev` on Debian/Ubuntu)

## Build

```bash
g++ hash.cpp -o hash -lcrypto
```

## Usage

```bash
./hash <filename>
```

## Example Run

Input file `test.txt`:
```
hello linux i am mohammed
```

Command:
```bash
g++ hash.cpp -o hash -lcrypto
./hash test.txt
```

Output:
```
fd9f7494403e78fae6235a44d4dd49610bb01db2180bbbb609608e600d5e9ef5
```

This 64-character string is the file's SHA-256 fingerprint. Any
change to the file's content — even a single character — would
produce a completely different hash.

## Notes

- `SHA256_Init`, `SHA256_Update`, and `SHA256_Final` are marked
  deprecated as of OpenSSL 3.0 in favor of the newer `EVP`
  interface, but remain functional and widely used for
  learning purposes.
- This tool only *computes* a hash; it does not yet store or
  compare hashes over time. That comparison logic is the next
  step toward a full file integrity checker.

## Next Steps

- Store the computed hash alongside the file path in a
  reference file
- On subsequent runs, recompute the hash and compare it to the
  stored value
- Alert the user if the hash has changed, indicating the file
  was modified
