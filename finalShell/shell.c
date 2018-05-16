/*
 * Autores: Mateo Gregory (1629431) - Sofía Navas (1629571) - Luis Restrepo (1427086)
 * Archivo: Shell.c
 * Fecha: 8 de mayo de 2018
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

//--------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------SEPARAR EL COMANDO EN TOKENS--------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------//

//Cuenta la cantidad de tokens en el comando (cuenta un token cuando encuentra un espacio)
int countBySpace(char comando[]){
  char * puntero;
  int cont = 0;
  puntero = strtok(comando, " ");

  while(puntero != NULL){
    cont++;
    puntero = strtok(NULL, " ");
  }
  return cont;
}

//Cuenta si hay un pipe en el comando
int countPipeOcurrence(char comando[]){
  if(strchr(comando, '|')!=NULL){ 
    return 1;
  }
  return 0;
}

//Cuenta si hay un '>' en el comando
int countFluxOcurrence(char comando[]){
  if(strchr(comando, '>')!=NULL){ 
    return 1;
  }
  return 0;
}

//Separa el comando en dos subarreglos cuando encuentra el pipe o el '>'
void split(char comando[], char *slicedCommand[], char c[]){
  char * puntero; 
  int i = 0;
  puntero = strtok(comando, c);
  
  while(puntero != NULL){
    slicedCommand[i] = puntero;
    puntero = strtok(NULL, c);
    i++;
  } 
}

//Llena cada posicion del arreglo finalTokens con cada token presente en el comando
void finalCommandTokens(char * finalTokens[], char comando[]){
  char * puntero;   
  int i = 0;
  puntero = strtok(comando, " ");
  
  while(puntero != NULL){
    finalTokens[i] = puntero;
    puntero = strtok(NULL, " ");
    i++;
  }
  finalTokens[i]=NULL;
}

//--------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------EJECUCION DE COMANDOS-------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------//

void executeBasicCommand(char *tokens[], char comando[]){     
  execvp(tokens[0], tokens);
  printf("%s: no se encontró la orden\n", comando);
}

void executeCommandWithPipes(char *args1[], char *args2[]){  
  pid_t pid1, pid2;  
  int pipefd[2], status1, status2;
  
  pipe(pipefd);
  pid1 = fork();

  if (pid1==0){
    dup2(pipefd[1], STDOUT_FILENO); //Abre el extremo de escritura
    close(pipefd[0]); // Cierra el extremo de lectura ya que no se utilizara
    execvp(args1[0], args1);    
    printf("No se encontró la orden\n");
  }

  pid2 = fork();
  if(pid2 == 0){
    dup2(pipefd[0], STDIN_FILENO); //Abre el extremo de lectura
    close(pipefd[1]); //Cierra el extremo de escritura ya que no se utiliza
    execvp(args2[0], args2);
    printf("No se encontró la orden\n");
  }

  close(pipefd[0]);
  close(pipefd[1]);
  wait(NULL);
}

//Redirecciona la salida estandar a un archivo
void executeOutputFileCommand(char args[], char fileName[]){
    close(STDOUT_FILENO);
    open(fileName, O_EXCL|O_CREAT|O_WRONLY, S_IRWXU);      
    execute(args);      
}


//--------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------FUNCION DE CONTROL----------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------//

//Recibe el comando y dependiendo del tipo, lo separa y llama a los metodos que lo ejecutan
//Tipos: 1) sin pipes y sin flujo; 2) con pipes y sin flujo; 3) con flujo
void execute(char comando[]){
  int commandSize = strlen(comando);  
  int cantPipes = countPipeOcurrence(comando);
  int cantFluxCommand = countFluxOcurrence(comando);  
  char copy[commandSize];
  strcpy(copy, comando);

  if(cantPipes==0 && cantFluxCommand==0){     
    int cantTokens = countBySpace(copy);
    char *finalTokens[cantTokens+1];    
    finalCommandTokens(finalTokens, comando);   
    executeBasicCommand(finalTokens, comando);            
  }else if(cantPipes!=0 && cantFluxCommand==0){  	    
    char *slicedCommand[2];
    split(copy, slicedCommand, "|");    

    //separar tokens del primer subarreglo
    char copy1[strlen(slicedCommand[0])];
    strcpy(copy1, slicedCommand[0]);
    int sizeFirstArray = countBySpace(copy1);
    char *tokensFirstArray[sizeFirstArray+1];
    finalCommandTokens(tokensFirstArray, slicedCommand[0]);
    
    //separar tokens del segundo subarreglo
    char copy2[strlen(slicedCommand[1])];
    strcpy(copy2, slicedCommand[1]);
    int sizeSecondArray = countBySpace(copy2);
    char *tokensSecondArray[sizeSecondArray+1];
    finalCommandTokens(tokensSecondArray, slicedCommand[1]);    

    executeCommandWithPipes(tokensFirstArray, tokensSecondArray);
  }else if(cantFluxCommand==1){
  	char * slicedCommand[2];
  	split(copy, slicedCommand, ">");
  	slicedCommand[1] = strtok(slicedCommand[1], " ");  	
  	executeOutputFileCommand(slicedCommand[0], slicedCommand[1]);
  }
}
