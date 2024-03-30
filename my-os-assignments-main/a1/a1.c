#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
///Marcu Ariana 30222

void parse(int file_descriptor)
{
    short int VERSION , HEADER_SIZE , SECT_TYPE ; //2 bytes
    int SECT_OFFSET, SECT_SIZE; //4 bytes
    char NO_OF_SECTIONS; //1 byte
    char MAGIC[3]; ///0 1 2, unde pozitia 2 are '\0'
    char SECT_NAME[19]; ///18 + '\0'

    //MAGIC e ultimul si are 2 octeti, deci size-2
    off_t var;
    var = lseek(file_descriptor, 0, SEEK_END); //citesc de jos in sus
    lseek(file_descriptor, var-2, SEEK_SET);
    read(file_descriptor, MAGIC, 2);
    MAGIC[2] = '\0';
    if(strcmp(MAGIC, "Uj") != 0)
    {
        printf("ERROR\n");
        printf("wrong magic\n");
        return;
    }

    lseek(file_descriptor, var-2-2, SEEK_SET);
    read(file_descriptor, &HEADER_SIZE, 2);

    //de la inceput:
    lseek(file_descriptor, var-HEADER_SIZE, SEEK_SET);
    read(file_descriptor, &VERSION, 2);
    if(VERSION<17 || VERSION>56)
    {
        printf("ERROR\n");
        printf("wrong version\n");
        return;
    }

    read(file_descriptor, &NO_OF_SECTIONS, 1);
    if(NO_OF_SECTIONS<8 || NO_OF_SECTIONS>13)
    {
        printf("ERROR\n");
        printf("wrong sect_nr\n");
        return;
    }
   for(int i=0; i<NO_OF_SECTIONS; i++)
    {
        read(file_descriptor, SECT_NAME, 18);
        SECT_NAME[18] = '\0';
        read(file_descriptor,&SECT_TYPE,2);
        if(SECT_TYPE!=11 && SECT_TYPE!=87 && SECT_TYPE!=12 && SECT_TYPE!=71 && SECT_TYPE!=64 && SECT_TYPE!=80)
    	{
        	printf("ERROR\n");
        	printf("wrong sect_types\n");
        	return;
    	}
        read(file_descriptor, &SECT_OFFSET, 4);
        read(file_descriptor, &SECT_SIZE, 4);
    }

    /*if(strcmp(MAGIC, "Uj") != 0)
    {
        printf("ERROR\n");
        printf("wrong magic\n");
        return;
    }
    if(VERSION<17 || VERSION>56)
    {
        printf("ERROR\n");
        printf("wrong version\n");
        return;
    }
    if(NO_OF_SECTIONS<8 || NO_OF_SECTIONS>13)
    {
        printf("ERROR\n");
        printf("wrong sect_nr\n");
        return;
    }
    if(SECT_TYPE!=11 && SECT_TYPE!=87 && SECT_TYPE!=12 && SECT_TYPE!=71 && SECT_TYPE!=64 && SECT_TYPE!=80)
    {
        printf("ERROR\n");
        printf("wrong sect_types\n");
        return;
    }*/



    var = lseek(file_descriptor, 0, SEEK_END); //citesc de jos in sus
    lseek(file_descriptor, var-2, SEEK_SET);
    //mai fac o data citirea HEADER_SIZE SI MAGIC
    
    read(file_descriptor, MAGIC, 2);
    MAGIC[2]='\0';
    
    lseek(file_descriptor,var-4,SEEK_SET);
    read(file_descriptor, &HEADER_SIZE, 2);
    lseek(file_descriptor, var-HEADER_SIZE, SEEK_SET);
    
    read(file_descriptor,&VERSION,2);
    
    read(file_descriptor,&NO_OF_SECTIONS,1);
    printf("SUCCESS\nversion=%d\nnr_sections=%d\n", VERSION, NO_OF_SECTIONS);
    //acelasi for ca mai sus
    for(int i=0; i<NO_OF_SECTIONS; i++)
    {
    	printf("section%d:", i+1);
    	read(file_descriptor, SECT_NAME, 18);
        SECT_NAME[18] = '\0';
        printf(" %s", SECT_NAME);
        read(file_descriptor, &SECT_TYPE, 2);
        printf(" %d", SECT_TYPE);
        read(file_descriptor, &SECT_OFFSET, 4);
        read(file_descriptor, &SECT_SIZE, 4);
        printf(" %d\n", SECT_SIZE);
    }
}

void list(char* path, int is_rec, char* name_starts_with, char* permissions)
{
    DIR* director = NULL;
    struct dirent* intrare = NULL;
    char FullPath[512];
    struct stat StatBuf;  //structura de la 5.2.3.2 laborator

    director = opendir(path);
    if(director == NULL)
    {
        perror("Nu s-a putut deschide directorul!\n");
        return;
    }

    while((intrare = readdir(director)) != NULL)
    {
        if(strcmp(intrare->d_name, ".") != 0 && strcmp(intrare->d_name, "..") != 0)
        {
            snprintf(FullPath, 512, "%s/%s", path, intrare->d_name);
            if(lstat(FullPath, &StatBuf) == 0)
            {
                if(strcmp(name_starts_with, "") == 0 && strcmp(permissions, "") == 0)
                {
                    printf("%s\n", FullPath);
                }
                else
                {
                    int len = strlen(name_starts_with);
                    if(strcmp(name_starts_with, "") != 0 && strncmp(intrare->d_name, name_starts_with, len) == 0)
                    {
                        if(strcmp(permissions, "") != 0) //st_mode e pt permisiuni fisier, si aplic AND logic
                        {
                            ///rwxrw-r--
                            char p[10];
                            p[0] = (StatBuf.st_mode & S_IRUSR ? 'r' : '-');
                            p[1] = (StatBuf.st_mode & S_IWUSR ? 'w' : '-');
                            p[2] = (StatBuf.st_mode & S_IXUSR ? 'x' : '-'); //read, write, execute pt owner ul fisierului

                            p[3] = (StatBuf.st_mode & S_IRGRP ? 'r' : '-');
                            p[4] = (StatBuf.st_mode & S_IWGRP ? 'w' : '-');
                            p[5] = (StatBuf.st_mode & S_IXGRP ? 'x' : '-'); //read, write, execute pt membrii grupului

                            p[6] = (StatBuf.st_mode & S_IROTH ? 'r' : '-');
                            p[7] = (StatBuf.st_mode & S_IWOTH ? 'w' : '-');
                            p[8] = (StatBuf.st_mode & S_IXOTH ? 'x' : '-'); //read, write, execute pentru restul utilizatorilor

                            p[9] ='\0';

                            if(strcmp(p, permissions) != 0)
                            {
                                printf("%s\n", FullPath);
                            }
                        }
                        else
                        {
                            printf("%s\n", FullPath);
                        }
                    }
                    if(strcmp(name_starts_with, "") == 0)
                    {
                        if(strcmp(permissions, "") != 0)
                        {
                            ///rwxrw-r--
                            char p[10];
                            p[0] = (StatBuf.st_mode & S_IRUSR ? 'r' : '-');
                            p[1] = (StatBuf.st_mode & S_IWUSR ? 'w' : '-');
                            p[2] = (StatBuf.st_mode & S_IXUSR ? 'x' : '-');

                            p[3] = (StatBuf.st_mode & S_IRGRP ? 'r' : '-');
                            p[4] = (StatBuf.st_mode & S_IWGRP ? 'w' : '-');
                            p[5] = (StatBuf.st_mode & S_IXGRP ? 'x' : '-');

                            p[6] = (StatBuf.st_mode & S_IROTH ? 'r' : '-');
                            p[7] = (StatBuf.st_mode & S_IWOTH ? 'w' : '-');
                            p[8] = (StatBuf.st_mode & S_IXOTH ? 'x' : '-');

                            p[9] ='\0';

                            if(strcmp(p, permissions) == 0)
                            {
                                printf("%s\n", FullPath);
                            }
                        }
                        else
                        {
                            printf("%s\n", FullPath);
                        }
                    }

                }

                if(is_rec && S_ISDIR(StatBuf.st_mode))
                {
                    list(FullPath, is_rec, name_starts_with, permissions);
                }
            }
        }
    }
    closedir(director);
}

int main(int argc, char **argv) //argc=cati argv sunt
{
    if(argc >= 2)
    {
        if(strcmp(argv[1], "variant") == 0)
        {
            printf("25012\n");
        }
        else if(strcmp(argv[1], "list") == 0)
        {
            char PATH[500] = "";
            int is_rec = 0; //listare nerecursiva
            char name_starts_with[100]="";
            char permissions[100]="";
            for(int i=2; i<argc; i++)
            {
                if(strncmp(argv[i], "path=", 5) == 0)
                {
                    strcpy(PATH, argv[i] + 5);
                }
                if(strcmp(argv[i], "recursive") == 0)
                {
                    is_rec = 1;
                }
                if(strstr(argv[i], "name_starts_with=") != NULL)
                {
                    sscanf(argv[i], "name_starts_with=%s", name_starts_with);
                }
                if(strstr(argv[i], "permissions=") != NULL)
                {
                    sscanf(argv[i],"permissions=%s", permissions);
                }
            }
            struct stat statbuf;
            if(stat(PATH, &statbuf) < 0 || !S_ISDIR(statbuf.st_mode))
            {
                printf("ERROR\n");
                printf("invalid directory path\n");
            }
            else
            {
                printf("SUCCESS\n");
                list(PATH, is_rec, name_starts_with, permissions);
            }
        }
        else if(strcmp(argv[1], "parse") == 0)
        {
            int file_descriptor;
            char *PATH;
            PATH = argv[2] + 5;
            if((file_descriptor = open(PATH, O_RDONLY)) >= 0)
            {
                 parse(file_descriptor);
                 close(file_descriptor);
            }
        }
    }
    return 0;
}
