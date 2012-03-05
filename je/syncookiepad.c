#include <stdio.h>
#include <err.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PAD_SIZE 12

u_int32_t pad[PAD_SIZE][256];

u_int32_t syn_cookie_pad( u_int32_t pad[PAD_SIZE][256], u_int32_t srcip, u_int16_t srcport, u_int32_t dstip, u_int16_t dstport ) {
	u_int32_t cookie;

	cookie = pad[0][(u_char) (srcip >> 24)];
	cookie ^= pad[1][(u_char) ((srcip >> 16) & 0x000000FF)];
	cookie *= pad[2][(u_char) ((srcip >> 8) & 0x000000FF)];
	cookie ^= pad[3][(u_char) (srcip & 0x000000FF)];

	cookie *= pad[4][(u_char) (srcip >> 24)];
	cookie ^= pad[5][(u_char) ((srcip >> 16) & 0x000000FF)];
	cookie ^= pad[6][(u_char) ((srcip >> 8) & 0x000000FF)];
	cookie ^= pad[7][(u_char) (srcip & 0x000000FF)];

	cookie *= pad[8][(u_char) (srcport & 0x000000FF)];
	cookie ^= pad[9][(u_char) (( srcport >> 8 ) & 0x000000FF)];
	cookie ^= pad[10][(u_char) (dstport & 0x000000FF)];
	cookie ^= pad[11][(u_char) (( dstport >> 8 ) & 0x000000FF)];

	return cookie;
}

int main(int argc, char **argv) {
	int fd;
	char srcip_s[16], dstip_s[16];
	u_int32_t srcip, dstip;
	u_int32_t srcport, dstport;
	u_int32_t cookie;

	printf( "Pad size = %lu\n", sizeof( pad ) );

	if( ( fd = open( "/dev/urandom", O_RDONLY ) ) == -1 ) err( 1, "open urandom" );
	if( read( fd, &pad, sizeof( pad ) ) != sizeof( pad ) ) err( 1, "read urandom" );
	close( fd );

	printf( "Pad loaded with random data.\n" );

	while( scanf( "%s %u %s %u", (char *) &srcip_s, &srcport, (char *) &dstip_s, &dstport ) == 4 ) {
		srcip = inet_addr( (char *) &srcip_s );
		dstip = inet_addr( (char *) &dstip_s );
		cookie = syn_cookie_pad( pad, srcip, srcport, dstip, dstport );
		printf( "%u\n", cookie );
	}
	return 0;
}
