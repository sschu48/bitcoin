#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>

int main() {
    // input string to hash
    const char *input = "test";
    unsigned char hash[EVP_MD_size(EVP_sha256())];   // buffer for 32-byte SHA-256 hash
    unsigned int hash_len; // to store actual hash length
    EVP_MD_CTX *ctx; // EVP context for hashing

    // create and initialize EVP context
    ctx = EVP_MD_CTX_new();
    if (ctx == NULL) {
        printf("Error: Failed to create EVP context\n");
        return 1;
    }

    // initialize digest with SHA-256
    if (!EVP_DigestInit_ex(ctx, EVP_sha256(), NULL)) {
        printf("Error: Failed to initialize SHA-256\n");
        EVP_MD_CTX_free(ctx);
        return 1;
    }

    // update context with input data
    if (!EVP_DigestUpdate(ctx, input, strlen(input))) {
        printf("Erro: Failed to update hash\n");
        EVP_MD_CTX_free(ctx);
        return 1;
    }

    //additional update to context
    if (!EVP_DigestUpdate(ctx, "blockchain", strlen("blockchain"))) {
        printf("Erro: Failed to update hash\n");
        EVP_MD_CTX_free(ctx);
        return 1;
    }

    // finalize hash and store in hash buffer
    if (!EVP_DigestFinal_ex(ctx, hash, &hash_len)) {
        printf("Error: Failed to finalize hash\n");
        EVP_MD_CTX_free(ctx);
        return 1;
    }

    // free EVP context
    EVP_MD_CTX_free(ctx);

    // print hash as hex
    printf("SHA-256 hash of '%s': ", input);
    hash[32] = 0;
    for (unsigned int i = 0; i < hash_len; i++) { 
        printf("%02x", hash[i]);
    }
    printf("\n");

    return 0;
}