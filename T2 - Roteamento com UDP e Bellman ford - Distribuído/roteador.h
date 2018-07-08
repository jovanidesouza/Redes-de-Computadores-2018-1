//T2 de redes
//roteamento com UDP
//utilizando bellman ford distribuido

//Jovani de Souza, Davi Pegoraro

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>


#define NROTEADORES 6 // Quantidade de Roteadores BUFFER
#define MENSAGEM_SIZE 100 // Quantidade de caracteres em uma mensagem



typedef struct{//estrutura que cria um roteador para a rede
	int id, porta;
	char ip[30];
}roteador;

typedef struct{//estrutura que cria uma mensagem enviada/recebida por um roteador
	int origem, destino, tipo;//orig, dest, flag de tipo de msg
	char msg[MENSAGEM_SIZE];
	int vetordist[NROTEADORES];
}mensagem;

void die(char *s);//imprime erros
void criar_roteador(int ID);//cria roteador
void * recebe(void * nada);
void * envia_vetor(void * nada);
void criavetor();
void criatabela();
void copia_vetor_para_tabela(int id, int vetdist[NROTEADORES]);
void envia();
void send_next(int next);
void atualiza_vetor_vizinhos();
void imprime_tabela_roteamento();
void send_next_vetor(int next);
void atualiza_vetor();
int achaNext(int destiny);
