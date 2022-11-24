/*============================== Servidor basico UDP ==========================
  Este servidor UDP destina-se a mostrar os conteudos dos datagramas recebidos.
  O porto de escuta encontra-se definido pela constante SERV_UDP_PORT.
  Assume-se que as mensagens recebida sao cadeias de caracteres (ou seja,
  "strings").
===============================================================================*/

#include <stdio.h>
#include <winsock.h>

#pragma comment(lib, "ws2_32.lib")

#define SERV_UDP_PORT	6000
#define BUFFERSIZE		4096
#define IP_SIZE			20
#define MAX_RESPOSTA	4096

void Abort(const char* msg);

/*________________________________ main ________________________________________
*/

int main(int argc, char* argv[])
{
	SOCKET sockfd;
	int iResult, nbytes;			// inteiros para controlo de variaveis
	int result;
	int length_addr, source_port;
	char source_ip[IP_SIZE];
	struct sockaddr_in serv_addr, cli_addr1, cli_addr2;
	char buffer[BUFFERSIZE], resposta[MAX_RESPOSTA];
	WSADATA wsaData;

	/*=============== INICIA OS WINSOCKS ==============*/

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		getchar();
		exit(1);
	}

	/*============ CRIA O SOCKET PARA RECEPCAO/ENVIO DE DATAGRAMAS UDP ============*/

	if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
		Abort("Impossibilidade de abrir socket");

	/*=============== ASSOCIA O SOCKET AO  ENDERECO DE ESCUTA ===============*/

	/*Define que pretende receber datagramas vindos de qualquer interface de
	rede, no porto pretendido*/

	memset((char*)&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET; /*Address Family: Internet*/
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);  /*Host TO Network Long*/	//config para dizer que o sv esta a correr no localhost
	serv_addr.sin_port = htons(SERV_UDP_PORT);  /*Host TO Network Short*/

	/*Associa o socket ao porto pretendido*/

	if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
		Abort("Impossibilidade de registar-se para escuta");

	printf("<SER1> Servidor iniciado com o porto de escuta: %d\n", SERV_UDP_PORT);


	/*================ PASSA A ATENDER CLIENTES INTERACTIVAMENTE =============*/
	double receivedValue;

	while (1) {
		fprintf(stderr, "<SER1>A esperar datagrama do primeiro cliente...\n");
		length_addr = sizeof(cli_addr1);

		nbytes = recvfrom(sockfd, (char *)&receivedValue, sizeof(receivedValue), 0, (struct sockaddr*)&cli_addr1, &length_addr);	// recvfrom - complementar do sendTo - recebe a mensagem
		// recvfrom(socket, mensagem, tamanho da mensagem, tba, tba, tba)

		if (nbytes == SOCKET_ERROR)
			Abort("Erro na recepcao de datagrama");

		buffer[nbytes] = '\0'; /*Termina a cadeia de caracteres recebidos com '\0'*/
		source_port = ntohs(cli_addr1.sin_port); // Network to host short
		strcpy_s(source_ip, (char*)inet_ntoa(cli_addr1.sin_addr)); //network to ascii
		printf("\n");

		printf("\n<SER1>Mensagem recebida do primeiro cliente {IP: %s, port: %d}\n", source_ip, source_port);
		//sprintf(resposta, "%d", strlen(buffer));

		fprintf(stderr, "<SER1>A esperar datagrama do segundo cliente...\n");
		length_addr = sizeof(cli_addr2);
		nbytes = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cli_addr2, &length_addr);	// recvfrom - complementar do sendTo - recebe a mensagem
			
		if (nbytes == SOCKET_ERROR)
			Abort("Erro na recepcao de datagrama");

		buffer[nbytes] = '\0'; /*Termina a cadeia de caracteres recebidos com '\0'*/

		source_port = ntohs(cli_addr2.sin_port); // Network to host short
		strcpy_s(source_ip, (char*)inet_ntoa(cli_addr2.sin_addr)); //network to ascii

		printf("\n");

		printf("\n<SER1>Mensagem recebida do segundo cliente {IP: %s, port: %d}\n", source_ip, source_port);
			
		nbytes = sendto(sockfd, (char*)&cli_addr1, sizeof(cli_addr1), 0, (struct sockaddr*)&cli_addr2, sizeof(cli_addr2));

		if (nbytes == SOCKET_ERROR) {
			printf("\n<SER1>Erro ao reenviar a mensagem ao cliente\n");
		}
		else {
			printf("\n<SER2>Primeiro cliente reencaminhado para o segundo cliente\n");
		}
	}

}

/*________________________________ Abort________________________________________
  Mostra uma mensagem de erro e o código associado ao ultimo erro com Winsocks.
  Termina a aplicacao com "exit status" a 1 (constante EXIT_FAILURE)
________________________________________________________________________________*/

void Abort(const char* msg)
{
	fprintf(stderr, "<SERV>Erro fatal: <%s> (%d)\n", msg, WSAGetLastError());
	exit(EXIT_FAILURE);

}