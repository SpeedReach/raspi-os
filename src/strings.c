int strcmp(const char* s1, const char* s2){
    while(*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void strcpy(char* dest, const char* src, int sourceLen){
    if(sourceLen == 0){
        dest [0] = '\0';
        return;
    }

    int index = 0;
    while (1)
    {
        char c = src[index];
        dest[index] = c;
        if(c == '\0'){
            break;
        }
        index ++;
        if(index >= sourceLen){
            dest[index] = '\0';
            break;
        }
    }
    return;
}
