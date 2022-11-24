/*=========================== Cliente basico TCP ===============================
Este cliente destina-se a enviar mensagens passadas na linha de comando, sob
a forma de um argumento, para um servidor especifico cujo socket e' fornecido atrav�s
da linha de comando. Tambem e' aguarda confirmacao (trata-se do comprimento da mensagem).

O protocolo usado e' o TCP.
==============================================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock.h>

#pragma comment(lib, "Ws2_32.lib")

#define BUFFERSIZE     4096
#define IP_SIZE		   15

void Abort(char* msg, SOCKET s);

/*________________________________ main ________________________________________
*/

int main(int argc, char* argv[]) {

	SOCKET sock = INVALID_SOCKET;
	int msg_len, nbytes, iResult;
	struct sockaddr_in serv_addr;
	char buffer[BUFFERSIZE], ip[IP_SIZE], port[IP_SIZE];
	WSADATA wsaData;
	/*
	if (argc != 4) { /*Testa sintaxe
		fprintf(stderr, "<CLI> Sintaxe: %s \"frase_a_enviar\" ip_destino porto_destino\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	*/

	if (argc != 1) {
		fprintf(stderr, "<CLI> Sintaxe: %s\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/*=============== INICIA OS WINSOCKS ==============*/
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		getchar();
		exit(1);
	}

	/*=============== ABRE SOCKET PARA CONTACTAR O SERVIDOR ==============*/
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
		Abort("Impossibilidade de abrir socket", sock);

	/*================= PREENCHE ENDERECO DO SERVIDOR ====================*/
	printf("IP: ");
	fgets(ip, IP_SIZE, stdin);
	ip[strlen(ip) - 1] = '\0';

	printf("PORT: ");
	fgets(port, IP_SIZE, stdin);

	memset((char*)&serv_addr, 0, sizeof(serv_addr));	/*a zero todos os bytes*/
	serv_addr.sin_family = AF_INET;				/*Address Family - Internet*/
	serv_addr.sin_addr.s_addr = inet_addr(ip);
	serv_addr.sin_port = htons(atoi(port));

	/*========================== ESTABELECE LIGACAO ======================*/
	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
		Abort("Impossibilidade de estabelecer ligacao", sock);
	do {
		/*====================== ENVIA MENSAGEM AO SERVIDOR ==================*/
		fflush(stdin);
		printf("Message: ");
		fgets(buffer, BUFFERSIZE, stdin);
		buffer[strlen(buffer) - 1] = '\0';
		msg_len = strlen(buffer);

		if ((nbytes = send(sock, buffer, msg_len, 0)) == SOCKET_ERROR)
			Abort("Impossibilidade de transmitir mensagem...", sock);
		else if (nbytes < msg_len)
			fprintf(stderr, "<CLI> Mensagem truncada...\n");
		else
			fprintf(stderr, "<CLI> Mensagem \"%s\" enviada\n", buffer);

		/*========================== ESPERA CONFIRMACAO =======================*/
		nbytes = recv(sock, buffer, sizeof(buffer), 0);

		if (nbytes == SOCKET_ERROR)
			Abort("Impossibilidade de receber confirmacao", sock);

		buffer[nbytes] = '\0';

		printf("<CLI> Confirmacao recebida {%s}.\n", buffer);

	} while (strcmp(buffer, "sair"));
	/*=========================== FECHA SOCKET ============================*/
	closesocket(sock);

	exit(EXIT_SUCCESS);
}


/*________________________________ Abort________________________________________
Mostra a mensagem de erro associada ao ultimo erro dos Winsock e abandona com
"exit status" a 1
_______________________________________________________________________________
*/
void Abort(char* msg, SOCKET s)
{
	printf("Erro - %s\n", msg);
	fprintf(stderr, "<CLI> Erro fatal: <%d>\n", WSAGetLastError());

	if (s != INVALID_SOCKET)
		closesocket(s);

	exit(EXIT_FAILURE);
}

