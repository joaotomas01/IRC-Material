/*=========================== Cliente basico UDP ===============================
Este cliente destina-se a enviar mensagens passadas na linha de comando, sob
a forma de um argumento, para um servidor especifico cuja locacao e' dada
pelas seguintes constantes: SERV_HOST_ADDR (endereco IP) e SERV_UDP_PORT (porto)

O protocolo usado e' o UDP.
==============================================================================*/

#include <winsock.h> // socket para windows - conexao entre maquinas cliente-servidor
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib") // conexao com library do winsock

#define SERV_HOST_ADDR "255.255.255.255" // localhost: "127.0.0.1" 
#define SERV_UDP_PORT  6000		   // porto onde vai ser enviada a mensagem - ha varios portos

#define BUFFERSIZE     4096		   // tamanho da mensagem
#define IP_SIZE			20

void Abort(const char* msg);	   // envia mensagens quando ha erro

/*________________________________ main _______________________________________
*/

int main(int argc, char* argv[])
{
	SOCKET sockfd;					
	WSADATA wsaData;				// estrutura da lib do winsock - armazena informacoes acerca da versao da library - padrao (irrelevante)

	int iResult;
	int msg_len;
	int nbytes;
	int length_addr;
	int cli_addr_len;
	int response_addr_len;
	int server_port;
	int enableBroadcast;

	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	struct sockaddr_in response_addr;
	struct sockaddr_in clientPair_addr;

	char buffer[BUFFERSIZE];		// armazena temporariamente a mensagem
	char server_address[IP_SIZE];
	char response_address[IP_SIZE];

	/*========================= TESTA A SINTAXE =========================*/

	if (argc != 7) {
		fprintf(stderr, "Sintaxe: %s -msg <mensagem> -ip <server_address> -port <server_port>\n", argv[0]);
		getchar(); //system("pause");
		exit(EXIT_FAILURE);
	}

	/*========================= VERIFICA AS FLAGS =======================*/

	for (int i = 0; i < argc; i++) {
		if (!strcmp(argv[i], "-msg") && i < argc - 1) { // procura a flag -msg
			strcpy_s(buffer, argv[i + 1]);	// guarda a mensagem no buffer
			buffer[strlen(buffer)] = '\0';
		}
		else if (!strcmp(argv[i], "-ip") && i < argc - 1) { // procura a flag -ip
			strcpy_s(server_address, argv[i + 1]);	// guarda o endereço no server_address
			server_address[strlen(server_address)] = '\0';
		}
		else if (!strcmp(argv[i], "-port") && i < argc - 1) { // procura a flag -port
			server_port = atoi(argv[i + 1]);	// guarda o porto no server_port
		}
	}

	/*=============== INICIA OS WINSOCKS ==============*/

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);	// inicializacao do winsock - padrao
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		getchar(); //system("pause");
		exit(1);
	}


	/*=============== CRIA SOCKET PARA ENVIO/RECEPCAO DE DATAGRAMAS ==============*/

	sockfd = socket(PF_INET, SOCK_DGRAM, 0);		 // criacao do socket / PF_INET - tipo de conexao (ipv4) / SOCK_DGRAM - UDP / 0
	// socket (tipo_conexao, tipo_protocolo, confirmacao)
	if (sockfd == INVALID_SOCKET)
		Abort("Impossibilidade de criar socket");


	/*========== ATIVA POSSIBILIDADE DE BROADCAST ========*/
	enableBroadcast = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (char*)&enableBroadcast, sizeof(enableBroadcast));


	/*================= PREENCHE ENDERECO DO SERVIDOR ====================*/

	memset((char*)&serv_addr, 0, sizeof(serv_addr)); /*Coloca a zero todos os bytes*/
	serv_addr.sin_family = AF_INET; /*Address Family: Internet*/											// familia endereço - ipv4
	serv_addr.sin_addr.s_addr = inet_addr(server_address); /*IP no formato "dotted decimal" => 32 bits*/	// servidor é o localhost (definido em cima)
	serv_addr.sin_port = htons(server_port); /*Host TO Netowork Short*/									// porto 
	

	/*====================== ENVIA MENSAGEM AO SERVIDOR ==================*/

	msg_len = strlen(buffer);	// tamanho da mensagem

	// ENVIA A MENSAGEM DO BUFFER COM O TAMANHO (msg_len + 1) PARA O SERVIDOR
	nbytes = sendto(sockfd, buffer, msg_len + 1, 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	if (nbytes == SOCKET_ERROR)
		Abort("O subsistema de comunicacao nao conseguiu aceitar o datagrama");

	printf("<CLI1> Mensagem enviada, sem confirmacao.\n");
	printf("<CLI1> A espera de resposta...\n");

	cli_addr_len = sizeof(cli_addr);

	if ((getsockname(sockfd, (struct sockaddr*)&cli_addr, &cli_addr_len) != SOCKET_ERROR)) {
		printf("<CLI1> Porto local automatico: %d\n", ntohs(cli_addr.sin_port));
	}

	length_addr = sizeof(serv_addr);
	response_addr_len = sizeof(response_addr);

	// RECEBE A MENSAGEM DO SERVIDOR OU DO SEGUNDO CLIENTE
	nbytes = recvfrom(sockfd, (char*)&clientPair_addr, sizeof(clientPair_addr), 0, (struct sockaddr*)&response_addr, &response_addr_len);
	if (nbytes == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAETIMEDOUT)
			Abort("Timeout!");
		else
			Abort("Erro na recepcao de datagrams");
	}
		
	if (nbytes != sizeof(clientPair_addr)) {
		Abort("Mensagem recebida de tipo inesperado");
	}
	
	// verificar se a mensagem recebida foi de um cliente ou de um servidor
	if (strcmp(server_address, (char*)inet_ntoa(response_addr.sin_addr)) == 0 && server_port == ntohs(response_addr.sin_port)) { // mensagem recebida do servidor
		
		// REENVIA A MENSAGEM PARA O PRIMEIRO CLIENTE
		nbytes = sendto(sockfd, (char*)&clientPair_addr, sizeof(clientPair_addr), 0, (struct sockaddr*)&clientPair_addr, sizeof(clientPair_addr));

		if (nbytes == SOCKET_ERROR)
			Abort("Erro ao enviar o endereço ao par remoto");

		printf("<CLI1> Sou o cliente 2\n");
		printf("<CLI1> O meu par remoto -> IP: %s, Porto: %d\n", inet_ntoa(clientPair_addr.sin_addr), ntohs(clientPair_addr.sin_port));
	}
	else {
		printf("<CLI1> Sou o cliente 1\n");
		printf("<CLI1> O meu par remoto -> IP: %s, Porto: %d\n", inet_ntoa(response_addr.sin_addr), ntohs(response_addr.sin_port));
	}

	buffer[nbytes] = '\0';

	/*========================= FECHA O SOCKET ===========================*/

	closesocket(sockfd);

	printf("\n");
	//getchar();
	exit(EXIT_SUCCESS);
}

/*________________________________ Abort________________________________________
  Mostra uma mensagem de erro e o código associado ao ultimo erro com Winsocks.
  Termina a aplicacao com "exit status" a 1 (constante EXIT_FAILURE)
________________________________________________________________________________*/

void Abort(const char* msg)
{

	fprintf(stderr, "<CLI1>Erro fatal: <%s> (%d)\n", msg, WSAGetLastError());
	exit(EXIT_FAILURE);

}
