#!/usr/bin/perl

use strict;
use warnings;
use IPC::Open2;

die "Unable to find syncookiepad" unless -x './syncookiepad';

my $count = $ARGV[0] || 20000;
my $bincount = $ARGV[1] || 53;
my @bins = ( 0 ) x $bincount;
my $worst_off = 100;

sub off {
	my( $test_bin ) = @_; 
	my $av = 0;
	for( my $bin = 0; $bin < $bincount; $bin++ ) {
		next if $bin == $test_bin;
		$av += $bins[$bin];
	}
	$av /= $bincount - 1;
	$av = int( ( $bins[$test_bin] / $av ) * 10000 ) / 100;
	if ( abs( $av - 100 ) > abs( $worst_off - 100 ) ) { $worst_off = $av; }
	return $av;
}

sub random_ip {
	return join( '.', int( rand( 254 ) ) + 1, int( rand( 255 ) ), int( rand( 255 ) ), int( rand( 255 ) ) );
}

my( $chld_out, $chld_in );
my $pid = open2( $chld_out, $chld_in, './syncookiepad' ) || die $!;
scalar <$chld_out>;
scalar <$chld_out>;

for( my $x = 0; $x < $count; $x++ ) {
	my $srcip = random_ip();
	my $dstip = random_ip();
	my $srcport = int(rand(65535)) + 1;
	my $dstport = int(rand(65535)) + 1;
	print $chld_in "$srcip $srcport $dstip $dstport\n";
	my $line = <$chld_out>;
	chop( $line );
	die "invalid data from syncookiepad" unless $line =~ /^\d+$/;
	#print STDOUT "$srcip $srcport $dstip $dstport => $line\n";

	$bins[$line % $bincount]++;
}

for( my $bin = 0; $bin < $bincount; $bin++ ) {
	print "$bin = $bins[$bin] (" . off( $bin ) . "%)\n";
}

print "\nTests = $count, Bins = $bincount, Worst Off = $worst_off%\n";
