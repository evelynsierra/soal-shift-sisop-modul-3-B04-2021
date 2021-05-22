#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <pthread.h>
#include <limits.h>


pthread_t tid[150];

typedef struct args_struct {
    char asal[500];
    char cwd[500];
} args_struct;

int flag = 1;

//cek apakah itu file
int is_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}


void movefileF(char *argv, char *cwd)
{
    int hiddenfile = 0;
    char string[500];
    strcpy(string, argv);

    //baca filenya
    int isfile = is_file(string);
    char* tipe = strrchr(string, '.');
    char* nama = strrchr(string, '/');

    if(nama[1] == '.')
    {
        hiddenfile = 1; //is hidden file
    }
    
    //lowercharacter
    char tipeLow[100];
    if(tipe)
    {
            strcpy(tipeLow, tipe);
            for(int i = 0; tipeLow[i]; i++){
                tipeLow[i] = tolower(tipeLow[i]);
            }
    }
    else{
        if(!isfile)
        {
            printf("This is folder\n");
        }
        else
        {
            strcpy(tipeLow, " Hidden"); // no extention
        }
    }

        if(hiddenfile)
        {
            strcpy(tipeLow," Hidden");
        }
        mkdir((tipeLow + 1), 0777); //folder with extention

        size_t len = 0 ;
        char a[500] ;
        strcpy(a, argv);
        char b[500] ;
        strcpy(b, cwd);
        strcat(b, "/");
        strcat(b, tipeLow+1);
        strcat(b, nama);
        if (!rename(a,b)) {
            printf("Berhasil Dikategorikan\n");
        }
        else
        {
            printf("Sad, Gagal :(\n");
        }
        remove(a);
    
}
void movefileD(char *argv, char *cwd)
{
    int hiddenfile = 0;
    char string[500];
    strcpy(string, argv);
    int isFile = is_file(string);
    char* tipe = strrchr(string,'.');
    char* nama = strrchr(string, '/');

    if(nama[1] == '.')
    {
        hiddenfile = 1;
    }

    char tipeLow[100];
    if(tipe)
    {
            strcpy(tipeLow, tipe);
            for(int i = 0; tipeLow[i]; i++){
                tipeLow[i] = tolower(tipeLow[i]);
            }
    }
    else
    {
        if(!isFile)
        {
            printf("This is a folder\n");
            return;
        }
        else
        {
            strcpy(tipeLow, " Unknown"); //no extention
        }
    }

    if(hiddenfile)
    {
        strcpy(tipeLow, " Hidden");
    }
    mkdir((tipeLow + 1), 0777);

     size_t len = 0 ;
        char a[1000] ;
        strcpy(a, argv);
        char b[1000] ;
        strcpy(b, cwd);
        strcat(b, "/");
        strcat(b, tipeLow+1);
        strcat(b, nama);
        if (rename(a,b)) 
        {
            flag=0;
        }

        remove(a);
}

void *fcommand(void* arg)
{
    args_struct args = *(args_struct*) arg;
    movefileF(args.asal, args.cwd);
    pthread_exit(0);

}

void *dcommand(void* arg)
{
    args_struct args = *(args_struct*) arg;
    movefileD(args.asal, args.cwd);
    pthread_exit(0);

}

void sortfile(char *asal)
{
    args_struct args;
    flag = 1;
    strcpy(args.cwd, "/Users/evelynsierra/ITS/Semester_4/Sisop/soal-shift-sisop-modul-3-B04-2021-main/soal3/soal3");
    DIR *dirp;
    struct dirent *entry;
    dirp = opendir(asal);
    int index = 0;
    while((entry = readdir(dirp)) != NULL)
    {
        if(entry->d_type == DT_REG)
        {
            char filename[305];
            sprintf(filename,"%s/%s",asal, entry->d_name);
            strcpy(args.asal,filename);

            if(strcmp(filename,"/Users/evelynsierra/ITS/Semester_4/Sisop/soal-shift-sisop-modul-3-B04-2021-main/soal3/soal3/soal3.c") != 0)
            {
                pthread_create(&tid[index], NULL, dcommand, (void *)&args);
                printf("%s\n", filename);
                sleep(1);
                index++;
            }
        }
    }

    if(!flag)
    {
        printf("Yah gagal disimpan\n");
    }
    else
    {
        printf("Direktori sudah disimpan\n");
    }
}

int main(int argc, char* argv[])
{
    args_struct args; //make struct
    getcwd(args.cwd, sizeof(args.cwd));

    if(strcmp(argv[1], "-f") == 0)
    {
        int count = 0;
        for (int i = 2; i < argc; i++)
        {
            strcpy(args.asal, argv[i]);
            //create thread
            pthread_create(&tid[count], NULL, fcommand, (void *)&args);
            sleep(1);
            count++;
        }
        for(int i = 0; i < count; i++)
        {
            pthread_join(tid[i], NULL);
        }
    }

    else if(strcmp(argv[1], "*") == 0)
    {
        char asal[] = "/Users/evelynsierra/ITS/Semester_4/Sisop/soal-shift-sisop-modul-3-B04-2021-main/soal3/soal3";

        sortfile(asal);
    }

    else if(strcmp(argv[1], "-d") == 0)
    {
        char asal[1000];
        strcpy(asal,argv[2]);
        sortfile(asal);
    }
    else
    {
        printf("Perbaiki Argumen\n");
        return 0;
    }

    return 0;
}
