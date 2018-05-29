//T2 de redes
//roteamento com UDP
//utilizando bellman ford distribuido

//Jovani de Souza, Davi Pegoraro

#include "roteador.h"
int ID, sock;
roteador roteadores[NROTEADORES];//vetor de roteadores, usa paenas o ID, mas instancia um vetor para facilitar a leitura do arquivo
struct sockaddr_in si_me, si_other;



int main(int argc, char const *argv[]) {

  ID = atoi(argv[1]);//converte o parametro de entrada em um inteiro e passa para id

//trata erros no parametro do teclado
  if(ID<0 || ID >= NROTEADORES){
		die("Numero de roteador invalido\n");//garante que o id do roteador seja de 0 a 5
	}
	if(argc<2){
		die("insira o ID do roteador\n");//se não for passado o ID
	}else if(argc>2){
		die("insira apenas o ID do roteador 'de 0 a 5'");//se for passado mais que 1 parametro
	}
//fim do tratamento dos erros do teclado

  memset((char *) &si_other, 0, sizeof(si_other));//zera a estrutura
  si_other.sin_family = AF_INET; //Familia ipv4
  si_other.sin_addr.s_addr = htonl(INADDR_ANY); //Atribui o ip

  criar_roteador(ID);//carrega insformações do arquivo,(id, porta ,ip) cria socket e faz bind do socket com a porta
  sleep(5);


  return 0;
}

void criar_roteador(int ID){//insrancia roteador, cria socket e faz bind com a porta

	int i,id;

	id=ID;
  //abre o arquivo e carrega as informações do roteador
  FILE *r = fopen("roteador.config", "r");
  if(!r) die("Erro ao abrir o arquivo roteadores.config\n");
  for(i = 0; fscanf(r, "%d %d %s\n", &roteadores[i].id, &roteadores[i].porta, roteadores[i].ip) != EOF; i++);
  //le arquivo de configuração dos roteadores, e atribui para todos os ids, usa apena o id informado pelo terminal
	printf("%d %d %s\n", roteadores[id].id, roteadores[id].porta, roteadores[id].ip);
  fclose(r);

  //Cria o socket
  if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
    die("Erro ao criar Socket\n");
  }
  //Zera a estrutura
  memset((char *) &si_me, 0, sizeof(si_me));

  si_me.sin_family = AF_INET; //Familia
  si_me.sin_port = htons(roteadores[id].porta); //Porta em ordem de bytes de rede
  si_me.sin_addr.s_addr = htonl(INADDR_ANY); //Atribui o ip, qualquer ip aqui, sera substituido

  //bind socket a porta
  if( bind(sock , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1){
      die("erro no bind");
  }
}

void die(char *s){//função pra imprimir erros na tela
	perror(s);
	//exit(1);
}
