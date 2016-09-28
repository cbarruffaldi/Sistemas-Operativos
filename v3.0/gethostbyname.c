/* EXTRAIDO DE: www.ibm.com/developerworks/linux/tutorials/l-sock */


#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
// "tcerda-HP-Pavilion-dm1-Notebook-PC"
int main(int argc, char *argv[]) {
	struct hostent *host; /* host information */
	struct in_addr h_addr;

	if (argc != 2) {
		fprintf(stderr, "USAGE: nslookup <inet_address>\n");
		return 1;
	}

	if ((host = gethostbyname(argv[1])) == NULL) {
		perror("error de gethostbyname\n");
		return 1;
	}

	h_addr.s_addr = *((unsigned long *) host-> h_addr_list[0]);
	printf("Dirección de IP: %s\n", inet_ntoa(h_addr));

	return 0;
}
