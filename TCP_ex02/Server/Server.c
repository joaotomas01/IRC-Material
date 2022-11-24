/*________________________________servidorTCPv1.c___________________________________*/
/*======================= Servidor interactivo TCP ============================
Este servidor destina-se mostrar mensagens recebidas via TCP, no porto
definido pela constante SERV_TCP_PORT.
Trata-se de um servidor que envia confirmacao (o comprimento, em bytes, da
mensagem recebida).
===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>

#pragma comment(lib, "Ws2_32.lib")

#define SERV_TCP_PORT  6000
#define BUFFERSIZE     4096

void Abort(char* msg, SOCKET s);
void AtendeCliente(SOCKET sock);
int writeN(SOCKET sock, char* buffer, int nBytes);
int readLine(SOCKET sock, char* buffer, int tamMax);

/*________________________________ main ________________________________________
*/
int main(int argc, char* argv[]) {

	SOCKET sock = INVALID_SOCKET, newSock = INVALID_SOCKET;
	int iResult;
	int cliaddr_len;
	struct sockaddr_in cli_addr, serv_addr;
	WSADATA wsaData;

	/*=============== INICIA OS WINSOCKS ==============*/
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		getchar();
		exit(1);
	}

	/*================== ABRE SOCKET PARA ESCUTA DE CLIENTES ================*/
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
		Abort("Impossibilidade de abrir socket", sock);

	/*=================== PREENCHE ENDERECO DE ESCUTA =======================*/
	memset((char*)&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);  /*Recebe de qq interface*/
	serv_addr.sin_port = htons(SERV_TCP_PORT);  /*Escuta no porto Well-Known*/

	/*====================== REGISTA-SE PARA ESCUTA =========================*/
	if (bind(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
		Abort("Impossibilidade de registar-se para escuta", sock);

	/*============ AVISA QUE ESTA PRONTO A ACEITAR PEDIDOS ==================*/
	if (listen(sock, 5) == SOCKET_ERROR)
		Abort("Impossibilidade de escutar pedidos", sock);

	/*================ PASSA A ATENDER CLIENTES INTERACTIVAMENTE =============*/
	cliaddr_len = sizeof(cli_addr);
	while (1) {
		/*====================== ATENDE PEDIDO ========================*/
		if ((newSock = accept(sock, (struct sockaddr*)&cli_addr, &cliaddr_len)) == SOCKET_ERROR)
			fprintf(stderr, "<SERV> Impossibilidade de aceitar cliente...\n");
		else {
			printf("<SERV> Ligacao aceite de {%s:%d}\r\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

			AtendeCliente(newSock);
			closesocket(newSock);
		}
	}
}

/*___________________________ AtendeCliente ____________________________________
Atende cliente.
______________________________________________________________________________*/

void AtendeCliente(SOCKET sock) {
	static char buffer[BUFFERSIZE];
	static unsigned int cont = 0;
	int nbytes, nBytesSent;
	char msg[BUFFERSIZE];

	/*==================== PROCESSA PEDIDO ==========================*/
	do {
		switch ((nbytes = readLine(sock, buffer, BUFFERSIZE, 0))) {	//em vez do rcv temos readLine

		case SOCKET_ERROR:
			fprintf(stderr, "\n<SER> Erro na recepcao de dados...\n");
			return;

		case  0:
			fprintf(stderr, "\n<SER> O cliente nao enviou dados...\n");
			return;

		default:
			buffer[nbytes] = '\0';
			printf("\n<SER> Mensagem n. %d recebida {%s}\n", ++cont, buffer);

			strcpy_s(msg, BUFFERSIZE, buffer);

			/*============ ENVIA CONFIRMACAO =============*/
			printf("<SER> Confirma recepcao de mensagem.\n");
			sprintf_s(buffer, BUFFERSIZE, "%d", nbytes);
			nbytes = strlen(buffer);

			if ((nBytesSent = writeN(sock, buffer, nbytes, 0)) == SOCKET_ERROR)	// em vez de send temos writeN
				fprintf(stderr, "<SER> Impossibilidade de Confirmar.\n");
			else if (nBytesSent < nbytes)
				fprintf(stderr, "<SER> Mensagem confirmada, mas truncada.\n");
			else
				printf("<SER> Mensagem confirmada.\n");
		}
	} while (strcmp(msg, "sair") != 0);

}

/*________________________________ Abort________________________________________
Mostra a mensagem de erro associada ao ultimo erro no SO e abando com
"exit status" a 1
_______________________________________________________________________________
*/
void Abort(char* msg, SOCKET s)
{
	fprintf(stderr, "\a<SER >Erro fatal: <%d>\n", WSAGetLastError());

	if (s != INVALID_SOCKET)
		closesocket(s);

	exit(EXIT_FAILURE);
}

int readLine(SOCKET sock, char* buffer, int nbytes) {
	int nread;
	int i;
	char c;
	i = 0;

	while (i < nbytes - 1) {
		nread = recv(sock, &c, sizeof(c), 0);
		if (nread == SOCKET_ERROR)
			return nread;
		if (nread == 0)
			break;
		if (c == '\r')
			continue;
		if (c == '\n')
			break;
		buffer[i++] = c;
	}
	buffer[i] = '\0';
	return i;	// return ao numero de bytes que leu
}

int writeN(SOCKET sock, char* buffer, int nbytes) {
	int nleft, nwritten;
	nleft = nbytes;
	while (nleft > 0) {
		nwritten = send(sock, buffer, nleft, 0);
		if (nwritten == 0 || nwritten == SOCKET_ERROR)
			return(nwritten);
		nleft = nwritten;
		buffer += nwritten;
	}
	return(nbytes);
}