//routingnetwork com sockets UDP
//Desenvolvido por Jovani de Souza 
//arquivo com defines e strucsts usadas


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


#define NROTEADORES 6 // Quantidade de Roteadores
#define NMENSAGEM 100 // Tamanho da caixa de mensagem do roteador
#define MENSAGEM_SIZE 100 // Quantidade de caracteres em uma mensagem
#define LINHAS 36


typedef struct{//estrutura que cria um roteador para a rede
	int id, porta;
	char ip[30];
	int msg_env, msg_rec;
}roteador;

typedef struct{//estrutura que cria uma mensagem enviada/recebida por um roteador
	int origem, destino, num;
	int rota[NROTEADORES];
	char msg[MENSAGEM_SIZE];
}mensagem;

typedef struct{
	int origem, destino, custo;
	int caminho[NROTEADORES];
}tabela_roteamento;

void die(char *s);//imprime erros
void prenchegrafo(int grafo[NROTEADORES][NROTEADORES]);//prenche o grafo com valores iniciais -1 e diagonal com 0
void inicializa_topologia(int grafo[NROTEADORES][NROTEADORES]);//abre arquivo enalces e carrega os valores no grafo
void test(int grafo[NROTEADORES][NROTEADORES]);//função de teste
void prenche_tabela(int grafo[NROTEADORES][NROTEADORES]);//cria tabela de roteamento e prenche com os caminhos
void dijkstra(int grafo[NROTEADORES][NROTEADORES] , int v_inicial);//recebe o grafo e o vertice inicial, calcula os menores caminhos
int verifica(int * vetor, int valor);//auxiliar do dijkstra
void removevetor(int * vetor, int valor);//auxiliar do dijkstra, remove um vertic do vetor de abertos
int achamenor(int * vetor, int * v_aberto);//auxiliar do dijkstra, acha menor vizinho de vertice
void criar_roteador(int argvID);
void backtracking(int origem, int anterior[NROTEADORES]);
int toint(char *str);
void * recebe(void * nada);
void envia(int argvID);
void send_next(int next);
void seta_rota(int origem, int destino);










