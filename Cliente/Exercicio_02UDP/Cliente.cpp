/*=========================== Cliente basico UDP ===============================
Este cliente destina-se a enviar mensagens passadas na linha de comando, sob
a forma de um argumento, para um servidor especifico cuja locacao e' dada
pelas seguintes constantes: SERV_HOST_ADDR (endereco IP) e SERV_UDP_PORT (porto)

O protocolo usado e' o UDP.
==============================================================================*/

#include <winsock.h> // socket para windows - conexao entre maquinas cliente-servidor
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib") // conexao com library do winsock

#define SERV_HOST_ADDR "127.0.0.1" // localhost: "127.0.0.1" stor: "10.65.134.137" 
#define SERV_UDP_PORT  6000		   // porto onde vai ser enviada a mensagem - ha varios portos

#define BUFFERSIZE     4096		   // tamanho da mensagem

void Abort(const char* msg);	   // envia mensagens quando ha erro

/*________________________________ main _______________________________________
*/

int main(int argc, char* argv[])	// argc - qtd de argumentos / argv - argumentos
{
	SOCKET sockfd;					// conexao entre sockets - estrutura de dados
	int msg_len, iResult;
	int nbytes, length_addr, cli_addr_len;
	struct sockaddr_in serv_addr, cli_addr;	// struct que armazena as informacoes do servidor - definir onde o cliente se vai conectar - ip, tipo de conexao (ipv4,ipv6...), porto
	char buffer[BUFFERSIZE];		// armazena temporariamente a mensagem
	WSADATA wsaData;				// estrutura da lib do winsock - armazena informacoes acerca da versao da library - padrao (irrelevante)

	/*========================= TESTA A SINTAXE =========================*/

	// se nao tiver dois argumentos o programa fecha
	if (argc != 2) {
		fprintf(stderr, "Sintaxe: %s frase_a_enviar\n", argv[0]);
		getchar(); //system("pause");
		exit(EXIT_FAILURE);
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

	/*================= PREENCHE ENDERECO DO SERVIDOR ====================*/

	memset((char*)&serv_addr, 0, sizeof(serv_addr)); /*Coloca a zero todos os bytes*/
	serv_addr.sin_family = AF_INET; /*Address Family: Internet*/											// familia endereço - ipv4
	serv_addr.sin_addr.s_addr = inet_addr(SERV_HOST_ADDR); /*IP no formato "dotted decimal" => 32 bits*/	// servidor é o localhost (definido em cima)
	serv_addr.sin_port = htons(SERV_UDP_PORT); /*Host TO Netowork Short*/									// porto 

	/*====================== ENVIA MENSAGEM AO SERVIDOR ==================*/

	msg_len = strlen(argv[1]);	// tamanho da mensagem

	// sendTo envia a mensagem para o servidor
	// sendTo (socket, mensagem, tamanho da mensagem (+1 nao preciso), tba, servidor, tamanho da info do servidor)

	if (sendto(sockfd, argv[1], msg_len + 1, 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
		Abort("O subsistema de comunicacao nao conseguiu aceitar o datagrama");

	printf("<CLI1> Mensagem enviada, sem confirmacao.\n");//... a entrega nao e' confirmada.\n");
	printf("<CLI1> A espera de resposta...\n");
	
	cli_addr_len = sizeof(cli_addr);
	if ((getsockname(sockfd, (struct sockaddr*)&cli_addr, &cli_addr_len) != SOCKET_ERROR)) {
		printf("<CLI1> Porto local automatico: %d\n", ntohs(cli_addr.sin_port));
	}

	length_addr = sizeof(serv_addr);
	//nbytes = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&serv_addr, &length_addr);
	nbytes = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
	if (nbytes == SOCKET_ERROR)
		Abort("Erro na recepcao de datagrams");
	buffer[nbytes] = '\0'; /*Termina a cadeia de caracteres recebidos com '\0'*/

	printf("<CLI1>Mensagem recebida {%s} do servidor", buffer);
	

	/*========================= FECHA O SOCKET ===========================*/

	closesocket(sockfd);

	printf("\n");
	getchar();
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
