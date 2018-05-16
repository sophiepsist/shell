/*
 * Autores: Mateo Gregory (1629431) - Sofia Navas (1629571) - Luis Restrepo (1427086)
 * Archivo: main.c
 * Fecha: 8 de mayo de 2018
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "shell.h"

int main(){ 
  while(1) {
    char comando[100];  
	printf("$ ");
	scanf(" %99[^\n]", comando);	
	if(!strcmp("exit", comando)){
		break;
	}

	pid_t pid = fork();
	if(!pid){
		execute(comando);		
		break;
  	}else{
  		wait(NULL);
  	} 
  }  
  return 0;
}