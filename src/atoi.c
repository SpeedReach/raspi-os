

int atoi(const char* str,int maxLength)
{
    int res = 0;
 
    for (int i = 0; str[i] != '\0' && i <maxLength; ++i)
        res = res * 10 + str[i] - '0';
 
    // return result.
    return res;
}