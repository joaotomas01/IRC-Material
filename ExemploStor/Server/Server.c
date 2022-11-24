/*_______________________________servidorTCP.C___________________________________*/
/*============================= Servidor concorrente TCP  =========================
Este servidor destina-se a colocar em contacto pares de clientes.
Forma os pares de forma sucessiva.
Em cada par, sempre que sao recebidos bytes num dos dois sockets,
estes são reencaminhados para o outro.
A conversa termina se:
	- no total, forem recebidos pelo menos LIMITE_CARACTERES caracteres;
	- passar um periodo de silencio de TIMEOUT segundos;
	- uma das ligacoes encerrar;
	- ocorrer um erro no acesso a um dos sockets.
=====================================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>

#define TIMEOUT		30   //segundos
#define BUFFERSIZE	10
#define MSG_BOAS_VINDAS "Servidor de dialogo\r\nAguarde...\r\n"
#define MSG_ARRANQUE_CONVERSA "Pode iniciar conversa com o seu par...\r\n"
#define LIMITE_CARACTERES  100

void AtendeCliente(LPVOID param);
int exchange(SOCKET s1, SOCKET s2);
void Abort(char* msg, SOCKET s);
int readLine(SOCKET sock, char* buffer, int nbytes);

/*________________________________ main ________________________________________
*/
int main(int argc, char* argv[]) {

	SOCKET sock = INVALID_SOCKET, newSock = INVALID_SOCKET;
	int iResult;
	struct sockaddr_in serv_addr;
	WSADATA wsaData;
	SECURITY_ATTRIBUTES sa;
	DWORD thread_id;
	SOCKET ligacoesAceites[2];
	SOCKET* parametrosThreadAtendeCliente;
	int contador;

	if (argc != 2) {
		fprintf(stderr, "<SERV> Usage: %s <porto de escuta>\n", argv[0]);
		exit(EXIT_SUCCESS);
	}

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("<SERV> WSAStartup failed: %d\n", iResult);
		getchar();
		exit(1);
	}

	/*
	 * a) Crie um socket TCP.
	 */

	if ((sock = /*...*/) == /*...*/)
		Abort("Impossibilidade de abrir socket", sock);

	/*=================== PREENCHE ENDERECO DE ESCUTA =======================*/
	memset((char*)&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	/*
	 * c) Associe o socket ao proto pretendido.
	 */

	if (/*...*/)
		Abort("Impossibilidade de registar-se para escuta", sock);

	if (listen(sock, 5) == SOCKET_ERROR)
		Abort("Impossibilidade de escutar pedidos", sock);

	printf("<SERV> Servidor de dialogo pronto no porto de escuta: %s\n", argv[1]);

	contador = 0;

	while (1) {

		/*
		 * d) Aguarde por/aceite pedidos de ligacao.
		 *    Nota: no presente exercício, não e' relevante conhecer a origem da ligacao acabada de aceitar.
		 */

		if ((newSock = /*...*/) == SOCKET_ERROR) {

			if (WSAGetLastError() == WSAEINTR)
				continue;

			fprintf(stderr, "<SERV> Impossibilidade de aceitar cliente...\n");

		}
		else {

			printf("<SERV> Novo cliente conectado\n");

			/*
			 * e) Envie ao cliente a string representada pela constante MSG_BOAS_VINDAS.
			 *	Nota: nao é necessario verificar se a operacao e' realizada com sucesso.
			 */

			/*...*/;

			ligacoesAceites[contador++] = newSock;

			if (contador == 2) {

				contador = 0;
				parametrosThreadAtendeCliente = (SOCKET*)malloc(2 * sizeof(SOCKET));

				if (parametrosThreadAtendeCliente == NULL) {

					printf("<SERV> Nao foi possivel reservar espaco para passar os parametros\n!");
					closesocket(ligacoesAceites[0]); closesocket(ligacoesAceites[1]);

				}
				else {

					parametrosThreadAtendeCliente[0] = ligacoesAceites[0];
					parametrosThreadAtendeCliente[1] = ligacoesAceites[1];

					sa.nLength = sizeof(sa);
					sa.lpSecurityDescriptor = NULL;

					if (CreateThread(&sa, 0, (LPTHREAD_START_ROUTINE)AtendeCliente, (LPVOID)parametrosThreadAtendeCliente, (DWORD)0, &thread_id) == NULL) {
						printf("<SERV> Nao foi possivel iniciar uma nova thread (error: %d)!\n", GetLastError());
						printf("<SERV> O par actual nao sera' atendido!\n");
						closesocket(ligacoesAceites[0]); closesocket(ligacoesAceites[1]);
					}

					printf("<SERV> Um novo par acaba de ser formado.\n");

				}
			}
		}
	}
}

/*___________________________ AtendeCliente ____________________________________
Atende cliente.
______________________________________________________________________________*/

void AtendeCliente(LPVOID param) {
	SOCKET sockCli1, sockCli2;
	struct sockaddr_in cli1Addr, cli2Addr;
	fd_set fdread, fdtemp;
	SOCKET* parSockets;
	int nbytes, totalCaracteres;
	struct timeval timeout = { TIMEOUT, 0 };

	totalCaracteres = 0;

	/*
	 * f) Incie sockCli1 e sockCli2 com base em param.
	 */
	parSockets = (/*...*/)param;
	sockCli1 = /*...*/;
	sockCli2 = /*...*/;

	free(parSockets);

	/*
	 * g) Envie a mensagem de arranque dada pela constante MSG_ARRANQUE_CONVERSA
	 *    aos dois clientes remotos.
	 */

	send(/*...*/);
	send(/*...*/);

	/*
	 * h) Inicie fdread de modo a poder testar sockCli1 e sockCl2 para efeitos
	 *    de leitura.
	 */

	 /*...*/

	while (1) {

		fdtemp = fdread;

		/*
		* i) Invoque a funcao select, passando-lhe fdtemp, com tempo de espero limite
		*    definido pela variavel timeout (esta ja' se encontra iniciada).
		*/

		switch (/*...*/) {

		case SOCKET_ERROR:

			if (WSAGetLastError() == WSAEINTR)
				break;

			fprintf(stderr, "<SERV_%d> Erro na rotina select (%d) ...\n", GetCurrentThreadId(), WSAGetLastError());
			closesocket(sockCli1); closesocket(sockCli2);
			return;

			/*
			 * j) Caso tenho ocorrido um timeout...
			 */

		case  /*...*/: //Timeout

			closesocket(sockCli1); closesocket(sockCli2);
			return;

		default:

			/*
			 * l) Teste sockCli1 e sockCli2 para efeitos de leitura
			 */

			if (/*...*/) {

				if ((nbytes = exchange(sockCli1, sockCli2)) <= 0) {
					closesocket(sockCli1); closesocket(sockCli2);
					return;
				}

				totalCaracteres += nbytes;

			}

			if (/*...*/) {

				if ((nbytes = exchange(sockCli2, sockCli1)) <= 0) {
					closesocket(sockCli2); closesocket(sockCli1);
					return;
				}

				totalCaracteres += nbytes;

			}

			if (totalCaracteres >= LIMITE_CARACTERES) {

				closesocket(sockCli1); closesocket(sockCli2);
				return;

			}

			break;

		} //switch
	} //while
}

/*_____________________________ exchange _______________________________________
Recebe caracteres do primeiro socket e escreve-o nos segundo

Devolve:
	SOCKET_ERROR : se houve erro
			   0 : EOF
			>= 0 : se transferiu algum byte
______________________________________________________________________________*/
int exchange(SOCKET s1, SOCKET s2)
{
	int nbytes;
	char buff[BUFFERSIZE];

	/*
	* n) Receba ate' BUFFERSIZE bytes em s1 e, caso tudo corra bem, escreva-os
	*    em s2. Recorra 'a variavel buff.
	*/

	if ((nbytes = recv(s1, /*...*/) > /*...*/) {
		nbytes = send(s2, /*...*/);
	}

	/*
	 * o) Teste se ocorreu uma situacao de fecho de ligacao.
	 */

	if (/*...*/) {
		fprintf(stderr, "<SERV_%d> Connection closed by foreign host\n", GetCurrentThreadId());
	}

	/*
	 * p) Teste se ocorreu uma situacao de erro de acesso aos sockets.
	 */

	if (/*...*/) {
		fprintf(stderr, "<SERV_%d> Erro no acesso para I/O a um dos sockets (%d)\n", GetCurrentThreadId(), WSAGetLastError());
	}

	return nbytes;
}

/*________________________________ Abort________________________________________
Mostra a mensagem de erro associada ao ultimo erro dos Winsock e abandona com
"exit status" a 1
_______________________________________________________________________________
*/
void Abort(char* msg, SOCKET s)
{
	fprintf(stderr, "\n<SERV_%d> Erro fatal: %s <%d>\n", GetCurrentThreadId(), msg, WSAGetLastError());

	if (s != INVALID_SOCKET)
		closesocket(s);

	exit(EXIT_FAILURE);
}
