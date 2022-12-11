#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ReaderFile()
{

    FILE *ptr;
    char ch;
    ptr=fopen("file.txt","r");

    if(ptr==NULL)
        printf("file can not be opened");

    while (ch!=EOF)
    {
        ch=fgetc(ptr);
        printf("%c\n",ch);
    }
    fclose(ptr);
}
void main()
{
    ReaderFile();


}
