#define _DEFAULT_SOURCE
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>

char *obtener_home(char *envp[]);
void parse_buffer(char buffer[], char *mi_argv[]);
void mi_cd(char *mi_argv[], char *home);
void mi_pwd();
void mi_ls(int p);
void mi_stat();
void imprimir_fecha_formateada(time_t t);
void mi_help();
int directorio_relativo(char *ruta);


int main(int argc, char *argv[], char *envp[]){
    char buffer[1024];
    int minish_exit = 0;
    ssize_t bytes_leidos;

    while (minish_exit != 1){
        write(1, "minish> ", 8);
        bytes_leidos = read(0, buffer, sizeof(buffer) - 1);
        char *mi_argv[64];
        char *home = obtener_home(envp);

        if (bytes_leidos > 0){
            buffer[bytes_leidos - 1] = '\0';
            parse_buffer(buffer, mi_argv);
            if (mi_argv[0] == NULL){
                continue;
            }

            if (strcmp(mi_argv[0], "exit") == 0){
                _exit(0);
            } else if (strcmp(mi_argv[0], "cd") == 0){
                mi_cd(mi_argv, home);
            } else if (strcmp(mi_argv[0], "pwd") == 0){
                mi_pwd();
            } else if (strcmp(mi_argv[0], "ls") == 0){
                int p = 0;
                if (mi_argv[1] != NULL && strcmp(mi_argv[1], "-1") == 0){
                    p = 1;
                }
                mi_ls(p);
            } else if(strcmp(mi_argv[0], "stats") == 0){
                mi_stats();
            } else if(strcmp(mi_argv[0], "help") == 0){
                mi_help();
            } else {
                write(2, "Comando no encontrado, usa help para ver la lista de comandos\n", 63);
            }
        }
    }

    return 0;
}

char *obtener_home(char *envp[]){
    char *home;

    int i = 0;
    while (envp[i] != NULL){
        if (envp[i][0] == 'H' && envp[i][1] == 'O' && envp[i][2] == 'M' &&
            envp[i][3] == 'E' && envp[i][4] == '='){
            home = &envp[i][5];
            break;
        }

        i++;
    }
    return home;
}

void parse_buffer(char buffer[], char *mi_argv[]){
    int i = 0;
    int k = 0;

    while (buffer[i] != '\0' && k < 63){
        while (buffer[i] == ' '){
            i++;
        }

        if (buffer[i] != '\0'){
            mi_argv[k] = &buffer[i]; // Guardamos la dirección de la primera letra
            k++;
        }

        while (buffer[i] != ' ' && buffer[i] != '\0'){
            i++;
        }

        if (buffer[i] == ' '){
            buffer[i] = '\0';
            i++;
        }
    }

    mi_argv[k] = NULL;
}

void mi_cd(char *mi_argv[], char *home){
    if (mi_argv[1] == NULL){
        chdir(home);
        return;
    }
    if (chdir(mi_argv[1]) != 0){
        write(2, "Error: No se encontro el directorio \n", 38);
        int length = 0;
        while (mi_argv[1][length] != '\0'){
            length++;
        }
        write(2, mi_argv[1], length);
        write(2, "\n", 1);
    }
}

void mi_pwd(){
    char path[1024];
    if (getcwd(path, sizeof(path)) != NULL){
        int length = 0;
        while (path[length] != '\0'){
            length++;
        }
        write(1, path, length);
        write(1, "\n", 1);
    }
}

void mi_ls(int p){
    DIR *dir = opendir(".");
    struct stat info;
    if (dir == NULL) {
        write(2, "Error al abrir el directorio\n", 29);
    } else {
        struct dirent *entrada;
        while ((entrada = readdir(dir)) != NULL) {
            int len_file = 0;
            while (entrada->d_name[len_file] != '\0'){
                len_file++;
            } 
            if (p == 0){
                write(1, entrada->d_name, len_file);
                write(1, " ", 1);
            } else {
                stat(entrada->d_name, &info);
                if (entrada->d_type == DT_DIR){
                    write(1, "Directorio ", 12);
                } else if (entrada->d_type == DT_REG){
                    write(1, "Archivo ", 8);
                }
                char size_str[20];
                int n = sprintf(size_str, "%ld", info.st_size);
                write(1, size_str, n);
                write(1, " ", 1);
                imprimir_fecha_formateada(info.st_mtime);
                write(1, " ", 1);
                write(1, entrada->d_name, len_file);
                write(1, "\n", 1);
            }

        }
        write(1, "\n", 1);
        closedir(dir);
    }
}

void imprimir_fecha_formateada(time_t t) {
    char buffer_fecha[20]; 
    

    struct tm *tm_info = localtime(&t);
    strftime(buffer_fecha, sizeof(buffer_fecha), "%Y%m%d %H:%M", tm_info);
    
    write(1, buffer_fecha, 14); 
}

void mi_stats(){
    DIR *dir = opendir(".");
    if (dir == NULL) {
        write(2, "Error al abrir el directorio\n", 29);
    } 

    struct dirent *entrada;
    int i = 0;
    int k = 0;

    while ((entrada = readdir(dir)) != NULL) {
        if (entrada->d_type == DT_DIR){
            i++;
        } else if (entrada->d_type == DT_REG){
            k++;
        }
    }
    closedir(dir);
    char i_str[12];
    char k_str[12];

    int len_i = sprintf(i_str, "%d", i);
    int len_k = sprintf(k_str, "%d", k);

    write(1, i_str, len_i);
    write(1, " Directorios\n", 13);
    write(1, k_str, len_k);
    write(1, " Archivos\n", 10);
}

void mi_help(){
    write(1, "Comandos disponibles:\n", 22);
    write(1, "cd <directorio> - Cambiar el directorio actual\n", 48);
    write(1, "pwd - Mostrar el directorio actual\n", 36);
    write(1, "ls <-1> - Listar archivos en el directorio actual\n", 49);
    write(1, "stat - Mostrar la cantidad de archivos y directorios en el directorio actual\n", 78);
    write(1, "help - Mostrar esta ayuda\n", 27);
}
