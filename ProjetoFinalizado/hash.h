#ifndef HASH_H
#define HASH_H

unsigned int hash2(const char *str) {
    unsigned int hash = 0;
    for (int i = 0; str[i]; i++)
        hash = (hash << 4) ^ (hash >> 28) ^ str[i];
    return hash;
}

#endif