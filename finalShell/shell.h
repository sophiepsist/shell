/*
 * Autores: Mateo Gregory (1629431) - Sofía Navas (1629571) - Luis Restrepo (1427086)
 * Archivo: Shell.h
 * Fecha: 8 de mayo de 2018
*/

int countBySpace(char comando[]);
int countPipeOcurrence(char comando[]);
int countFluxOcurrence(char comando[]);
void split(char comando[], char *slicedCommand[], char c[]);
void finalCommandTokens(char * finalTokens[], char comando[]);
void executeBasicCommand(char *tokens[], char comando[]);
void executeCommandWithPipes(char *args1[], char *args2[]);
void executeOutputFileCommand(char args[], char fileName[]);
void execute(char comando[]);
