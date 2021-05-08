#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<dirent.h>

pthread_t tid[100];
char tempwd[1024];

//mengecek file atau folder
int is_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

//get extention of the file, file name, and moving file
//opsi -f

void* move(void *arg){
    char *path;
    char ext[100], dirname[100], dom[100];
    char cut[100], fn[100];
    path = (char*) arg;
    strcpy(dom, arg);
    strcpy(cut, arg);

    char *split, *arr[5], *split2, *arrn[5];
    int n = 0;
    split = strtok(path,"."); //split path


    while(split != NULL)
    {
        arr[n] = split;
        n++;
        split = strtok(NULL,".");
    }

    if(n == 1)
    {
        strcpy(ext, "Unknown");
    }

    else
    {
        for(int a = 0; a < strlen(arr[n-1]); a++)
        {
            ext[a] = tolower(arr[n-1][a]); //menyimpan seluruh extention
        }
    }

    n = 0; 

    //split direktori
    split2 = strtok(cut, "/");
    while(split2 != NULL)
    {
        arrn[n] = split2;
        n++;
        split2 = strtok(NULL,"/");
    }

    strcpy(fn, arrn[n-1]);

    char cwd[1024];
    getcwd(cwd, sizeof(cwd)); //mendapatkan direktori yang sedang dibuka
    strcpy(dirname, cwd);
    strcat(dirname, "/");
    strcat(dirname, ext);
    memset(ext,0,100);
    mkdir(dirname,0777);

//read mode on old file and write mode on new file

    FILE *pf1, *pf2;
    pf1 = fopen(dom,"r");
    strcat(dirname, "/");
    strcat(dirname, fn);
    pf2 = fopen(dirname,"w");

    if(!pf1)
    {
        printf("Unable to open source file to read\n");
    }

    if(!pf2)
    {
        printf("Unable to open target file to write\n");
    }

    int ch;
    while((ch = fgetc(pf1)) != EOF)
    {
        fputc(ch, pf2); //memindahkan konten pada file lama
    }

    fclose(pf1);
    fclose(pf2);
    remove(dom); //menghapus file lama

    return NULL;
}

int main(int argc, char *argv[])
{
    getcwd(tempwd, sizeof(tempwd));

    int i;
    if(strcmp(argv[1], "-f") == 0)
    {
        for(int i = 2; i < argc; i++)
        {
            if(is_file(argv[i]))
            {
                pthread_create(&(tid[i-2]), NULL, move, (void *)argv[i]); //create thread
            }
        }
        for(int i = 0; i < argc - 2; i++)
        {
            pthread_join(tid[i], NULL);
        }
    }
}
