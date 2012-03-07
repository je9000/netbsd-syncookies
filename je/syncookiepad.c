#include <string.h>
#include <stdio.h>
#include <err.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>

static u_int16_t tcp_sc_msstab[] = { 8, 256, 468, 536, 996, 1452, 1460, 8960 };

typedef u_int32_t cookie_pad[6][256];

struct tcp_syn_cookie_data {
	int update_time;
	cookie_pad even_data;
	cookie_pad odd_data;
}; 

struct tcp_syn_cookie_data cookie_data;

u_int32_t syn_cookie_pad( int is_even, u_int32_t srcip, u_int16_t srcport, u_int32_t dstip, u_int16_t dstport, u_int16_t mss, u_int32_t irs ) {
	u_int32_t cookie;
	int msstab_entry;

	cookie_pad *pad = is_even ? &cookie_data.even_data : &cookie_data.odd_data;

	cookie = (*pad)[0][(u_char) (srcip >> 24)];
	cookie ^= (*pad)[1][(u_char) ((srcip >> 16) & 0x000000FF)];
	cookie *= (*pad)[2][(u_char) ((srcip >> 8) & 0x000000FF)];
	cookie ^= (*pad)[3][(u_char) (srcip & 0x000000FF)];

	cookie *= (*pad)[4][(u_char) (srcport & 0x000000FF)];
	cookie ^= (*pad)[5][(u_char) srcport >> 8];

	cookie += dstip ^ ( dstport << 16 );
	cookie += irs;

	for (msstab_entry = 0; msstab_entry < 7; msstab_entry++) {
        if (mss < tcp_sc_msstab[msstab_entry + 1])
            break;
    }

	cookie = ( cookie & 0xFFFFFFF8 ) | ( is_even << 3 ) | tcp_sc_msstab[msstab_entry];

	return cookie;
}

int main(int argc, char **argv) {
	char srcip_s[16], dstip_s[16];
	u_int32_t srcip, dstip;
	u_int32_t srcport, dstport;
	u_int32_t cookie;
	u_int32_t irs = 123456;
	int mss = 1400;

	printf( "Pad size = %lu\n", sizeof( cookie_pad ) );

	arc4random_stir();
	arc4random_buf( &cookie_data.even_data, sizeof( cookie_pad ) );
	arc4random_buf( &cookie_data.odd_data, sizeof( cookie_pad ) );

	printf( "Pad loaded with random data. Enter 'ip port ip port' to calculate cookies.\n" );
	fflush( stdout );

	while( scanf( "%s %u %s %u", (char *) &srcip_s, &srcport, (char *) &dstip_s, &dstport ) == 4 ) {
		srcip = inet_addr( (char *) &srcip_s );
		dstip = inet_addr( (char *) &dstip_s );
		cookie = syn_cookie_pad( 1, srcip, srcport, dstip, dstport, mss, irs );
		printf( "%u\n", cookie );
		fflush( stdout );
	}
	return 0;
}

