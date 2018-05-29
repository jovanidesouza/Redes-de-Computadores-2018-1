//Jovani de Souza & Davi Pegoraro
//roteamento com UDP

#include "roteador.h"


tabela_roteamento linhas[LINHAS];//vetor de linhas//cria a tabela de roteamento
int count_table=0;

roteador roteadores[NROTEADORES];//vetor de roteadores, usa paenas o ID, mas instancia um vetor para facilitar a leitura do arquivo

int op=-1, msg_control=0, msg_control_rec=0;
mensagem msgs_env[NMENSAGEM];//CRIA VETOR DE MENSAGENS, funciona como uma caixa de mensagens enviadas
mensagem msgs_rec[NMENSAGEM];//CRIA VETOR DE MENSAGENS, funciona como uma caixa de entrada

struct sockaddr_in si_me, si_other;

int sock;
int argvID;

pthread_t recebe_id;


//função main - vai receber por linha de comando o ID do roteador de vai variar de 0 a 5
int main(int argc, char *argv[]){

	
	int x;

	int grafo[NROTEADORES][NROTEADORES];//grafo que sera usado para gerar a tabela de roteamento 
	
	prenchegrafo(grafo);//função deixa a matriz preenchida com -1 e a diagonal com 0

	inicializa_topologia(grafo);//função que inicializa a topologia da rede baseada no arquivo de entrada enlaces.config

	//test(grafo);//teste de impressão

	prenche_tabela(grafo);

	argvID=toint(argv[1]);///cast

	//trata erro no ID passado pelo terminal

	if(argvID<0 || argvID >= NROTEADORES){
		die("Numero de roteador invalido\n");//garante que o id do roteador seja de 0 a 5
	}

	if(argc<2){
		die("insira o ID do roteador\n");//se não for passado o ID
	}else if(argc>2){
		die("insira apenas o ID do roteador 'de 0 a 5'");//se for passado mais que 1 parametro
	}


	memset((char *) &si_other, 0, sizeof(si_other));//zera a estrutura
  	si_other.sin_family = AF_INET; //Familia ipv4
  	si_other.sin_addr.s_addr = htonl(INADDR_ANY); //Atribui o ip

	criar_roteador(argvID);

	pthread_create(&recebe_id, NULL, recebe, NULL); //Cria a thread receptora

	sleep(2);

	

	while(1){//loop menu, usado pra mandar e ler mensagens
		system("clear");

		system("clear");
	    if(op == -1){
	      printf("ROTEADOR %d\n", argvID);
	      printf("---------------------------------------------\n");
	      printf("0 - Atualizar\n");
	      printf("1 - Checar Mensagens\n");
	      printf("2 - Enviar Mensagem\n");
	      printf("3 - Sair\n\n");
	      printf("---------------------------------------------\n");
	      scanf("%d", &op);
	    }else if(op==0){
	    	op=-1;
	    }else if(op==1){
	    
	    	for(x=0; x<=msg_control_rec; x++){
	    		if(x<msg_control_rec){
	    			printf("Mensagem %d recebida de %d\n",msgs_rec[x].num, msgs_rec[x].origem);
	    		}
	    		printf("%s\n", msgs_rec[x].msg);
	    	}
	    	sleep(10);
	    	op=-1;
	    }else if(op==2){
	    	envia(argvID);
	    	
	    }else if(op==3){
	    	exit(1);

	    }else{
	    	op=-1;
	    }


	}

}

void * recebe(void * nada){//recebe mensagens e retransmite se necessário

	
	int slen=sizeof(si_other);//si_me
	int next,i;

	while(1){
		if((recvfrom(sock, &msgs_rec[msg_control_rec], sizeof(msgs_rec[msg_control_rec]), 0, (struct sockaddr*) &si_me, &slen)) == -1){
			printf("Erro ao receber mensagem\n");
			msg_control_rec--;
		}else{

		}

		if(msgs_rec[msg_control_rec].destino== argvID){//checa se é pra mim
			printf("Mensagem recebida do roteador %d\n", msgs_rec[msg_control_rec].origem);
			msg_control_rec++;
		}else{
			//reenvia
			
			msgs_env[msg_control]=msgs_rec[msg_control_rec];//atribui a mensagem recebida como uma mensagem de envio
			for(i=0; i<NROTEADORES; i++){//procura na rota o indice atual, entao atrivui o rpximo para next
				if(msgs_env[msg_control].rota[i]==argvID){
					next=msgs_env[msg_control].rota[i+1];
				}
			}
			printf("\nRetransmitindo de %d para %d\n",msgs_rec[msg_control_rec].origem, next);
			send_next(next);//reenvia a mensagem para o destino
		}

	}

	
}

void envia(int argvID){//envia mensagens, faz busca e fornece rota para o pacote



	int i=1,dest,next;
	
	printf("Digite o roteador destino\n");
	scanf("%d", &dest);//destino da mensagem

	if(dest>5 || dest<0){
		die("Esse roteador nao existe\n");//trata erro na inserção do destino
		op=-1;
	}
	seta_rota(argvID, dest);//com orig & dest, busca o vetor rota para a mesagem[msg_control]

	printf("Escreva a mensagem que quer enviar para %d\n", dest);
	getchar();
	fgets(msgs_env[msg_control].msg, MENSAGEM_SIZE, stdin);//mensagem no vetor de msgs_env// pega do teclado e guarda a msg na caixa 
	msgs_env[msg_control].num=msg_control;//seta id da mensagem
	msgs_env[msg_control].origem= argvID;
	msgs_env[msg_control].destino= dest;

	//até aqui// pega destino, pega msg e seta a rota para a mensagem//msg prota para enviar

	next=msgs_env[msg_control].rota[1];//next recebe o segundo valor do vetor rota, pois a posição 0 contem o proprio roteador
	

	send_next(next);

	op=-1;

}

void send_next(int next){

	printf("Enviando mensagem para %d\n", next);
	sleep(2);

	si_other.sin_port= htons(roteadores[next].porta);
	if(inet_aton(roteadores[next].ip , &si_other.sin_addr) == 0){//se tiver erro no endereço
		die("Falha ao obter ip do destino\n");
	}else{
		if(sendto(sock, &msgs_env[msg_control],sizeof(msgs_env[msg_control]), 0,(struct sockaddr*) &si_other, sizeof(si_other)) ==-1){
			die("Falha no envio da mensagem\n");//se tiver erro na mensagem
		}else{
			printf("Roteador %d enviando Mensagem %d para %d\n", argvID, msgs_env[msg_control].num, roteadores[next].id);
		}
	}

}

void seta_rota(int origem, int destino){//recebe uma origem e um destino, retorna m vetor com a rota

	int i,j,a,b;

	msg_control++;

	a=origem;
	b=destino;


	for(i=0; i<LINHAS; i++){//faz uma busca na tabela de roteamento
		if(linhas[i].origem==a && linhas[i].destino==b){//se encontrar origem e destino compativeis, então
			for(j=0; j<NROTEADORES; j++){
				msgs_env[msg_control].rota[j]=linhas[i].caminho[j];//atribui o caminho encontrado, para a rota da mensagem
				//printf("%d\n", msgs_env[msg_control].rota[j]);
			}
			
		}
	}

	sleep(1);
	op=-1;
}

void criar_roteador(int argvID){

	int i,id;

	id=argvID;


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
  	si_me.sin_addr.s_addr = htonl(INADDR_ANY); //Atribui o ip
  	//si_me.sin_addr.s_addr = inet_addr(roteadores[id].ip); //Atribui o socket

  	//bind socket a porta
    if( bind(sock , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1){
        die("erro no bind");
    }

	//printf("%s\n", ID);
}




void prenche_tabela(int grafo[NROTEADORES][NROTEADORES]){

	int i,j,count=0;

	for(i=0; i<NROTEADORES; i++){//prenche a tabela de roteamento com as origens e destinos
		for(j=0; j<NROTEADORES; j++){
			linhas[count].origem=i;
			linhas[count].destino=j;
			count++;
		}
	}
	for(i=0; i<NROTEADORES; i++){
		dijkstra(grafo, i);
	}

	//sleep(500);


}

void inicializa_topologia(int grafo[NROTEADORES][NROTEADORES]){

	int x,y,custo;

	FILE *p = fopen("enlaces.config", "r");//abre o arquivo enlaces em modo leitura
	if(!p){
		die("Erro ao abrir arquivo enlaces.config\n");
	}
	while(fscanf(p, "%d %d %d\n", &x, &y, &custo) != EOF){//enquanto a leitura do arquivo não chega ao fim
		grafo[x][y]=custo;
		grafo[y][x]=custo;
	}
	fclose(p);
}

void prenchegrafo(int grafo[NROTEADORES][NROTEADORES]){//recebe um grafo NxN e preenche a diagona principal com 0 e o restante com -1

	int i,j;

	//memset(grafo, -1, sizeof(grafo));

	for(i=0; i<NROTEADORES; i++){//preenche o grafo com -1
		for(j=0; j<NROTEADORES; j++){
			grafo[i][j]=-1;
		}
		
	}

	for(i=0, j=0; i< NROTEADORES; i++, j++){//zera a diagonal
		grafo[i][j]=0;
	}

}

void die(char *s){//função pra imprimir erros na tela
	perror(s);
	//exit(1);
}

void test(int grafo[NROTEADORES][NROTEADORES]){

	int i,j;

	for(i=0; i<NROTEADORES; i++){//preenche o grafo com -1
		for(j=0; j<NROTEADORES; j++){
			printf("%d\t", grafo[i][j]);
		}
		printf("\n");
	}

}
//////////////////////////////////////////////////////////////////////

int verifica(int * vetor, int valor){//função que verifica a existencia de um valor no vetor de vertices abertos
  int a;                             //retorna 1 se encontrar o valor e retorna 0 se não encontrar
  for(a = 0; a < 6; a++){
    if(vetor[a]== valor)
        return 1;
  }
  return 0;
}

void removevetor(int * vetor, int valor){// função que verifica um valor no vetor, se encontrar o valor, substitui por 0, usado para liberar o vetor de abertos
  int a;
  for(a = 0; a < 6; a++){
    if(vetor[a] == valor)
      vetor[a] = 0;
  }
}

int achamenor(int * vetor, int * v_aberto){
  int a, b=0, menor = 1000;

  for(a = 0; a < 6; a++)
    if(vetor[a]!=0  && vetor[a] < menor && verifica(v_aberto, a)){//se o valor for diferente de 0, se o valor sor menor que 1000 e se o valor estiver no vetor de abertos
      menor = vetor[a];                                          //então menor recebe o novo valor
      b = a;
    }                                                           //no fim a função retorna esse menor valor
  return b;
}


void dijkstra(int grafo[NROTEADORES][NROTEADORES], int v_inicial){

    int v_aberto[NROTEADORES], i,a=v_inicial,b,controle=NROTEADORES;
    int vertices[NROTEADORES];
    int anterior[NROTEADORES];
    int distancia[NROTEADORES];
    int origem=v_inicial;

    for(i=0; i<NROTEADORES; i++){//atribui os valores dos nós para o vetor
        vertices[i]=i;
        v_aberto[i]=i;
        anterior[i]=i;
        distancia[i]=1000;//custos ficam em 1000
    }
    distancia[v_inicial]=0;//custo de vertici inicial fica 0


    while(controle >= 0){//calcula o dijkstra
      for(b = 0; b < NROTEADORES; b++){



        if(grafo[a][b]>0 && verifica(v_aberto,b) && (distancia[b] > (grafo[a][b]+distancia[a]))){// se o valor atual for diferente de 0 e o valor de b estiver no vetor de abertos e se o custo é menor que 1000
          distancia[b] = grafo[a][b] +distancia[a];// assume novo custo
          anterior[b] = a;//guarda vertice anterior

        }
      }
      removevetor(v_aberto, a);//remove o vertice do vetor de abertos
      controle--;
      a = achamenor(distancia, v_aberto);//a recebe o valor do menor vizinho
  	}

    backtracking(origem, anterior);

}

void backtracking(int origem, int anterior[NROTEADORES]){

  int a, x=0,y=0, v_rota[NROTEADORES],aux=0,destino=6;

    while(aux<=5){
      
      

       a=aux;  

      while(destino != origem){// usa v_rota para salvar o caminho de tras pra frente fazendo o backtracking
        destino = a;
        v_rota[x] = destino;
        x++;
        a = anterior[destino];
      }

      

      //printf("%d %d ", linhas[count_table].origem, linhas[count_table].destino);

      for(a = x-1,y=0; a >=0 ; a--){
        
        linhas[count_table].caminho[y]=v_rota[a];//guarda o valor do caminho no vetor da estrutura da tabela

        //printf(" %d ",linhas[count_table].caminho[y] );

        y++;

      }
      //printf("\n");
      //	sleep(1);
      count_table++;
      x=0;
      aux++;
      destino=aux;
    }

     // printf("\n");

}

int toint(char *str){//converte pra int//função da internet
  int i, pot, ans;
  ans = 0;
  for(i = strlen(str) - 1, pot = 1; i >= 0; i--, pot *= 10)
    ans += pot * (str[i] - '0');
  return ans;
}