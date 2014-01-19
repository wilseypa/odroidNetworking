#!/usr/bin/perl -w

use strict;
use warnings;

my $mode = shift || 'server';
my @sizes = (1, 2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96, 128, 192, 256, 384, 512, 768, 1024, 1536, 2048, 3072, 4096, 6144, 8192, 12288, 16384, 24576, 32768);
my $output_file = 'lat_tests.csv';

my $iterations = 1000;

my @tests = ('ib_send_lat', 'ib_write_lat');
my @connection_types = ('RC', 'UC');
my @options = ('', '-z');

my $target;
my $server;

if ( $mode =~ /server|target|slave/ ) {
  $target = '';
  $server = 1;
}
elsif ( $mode =~ /client|initiator|master/ ) {
  $target = ' ' . shift(@ARGV) . ' -U';
  $server = 0;
}
else {
  die "Invalid server/client specifier\n";
}

if ( !$server ) {
  open(OUT, ">$output_file") || die "Could not open $output_file for writing\n";
  print OUT "Benchmark,Message Size,Connection Type,Connection Specifics,Latency Results\n";
}

for my $test (@tests) {
  for my $size (@sizes) {
    for my $option (@options) {
      my $opt_string;
      if ( $option eq '' ) {
        $opt_string = 'IB Verbs Connection';
      } elsif ( $option eq '-z' ) {
        $opt_string = 'RDMA CM Connection with IB QPs';
      } elsif ( $option eq '-R' ) {
        $opt_string = 'RDMA CM QPs';
      }
      for my $type (@connection_types) {
        if ( !$server ) {
          sleep 1;
        }
        my $output = `$test -c $type $option -n $iterations -s $size $target`;
        if ( !$server ) {
          print OUT "${test},${size}B,${type},${opt_string},";
          my @lines = split("\n", $output);
          for (@lines) {
            if ( /^([0-9]+), ([0-9.]+)$/ ) {
              print OUT $2 . ',';
            }
          }
          print OUT "\n";
        }
      }
    }
  }
}

if ( !$server ) {
  close OUT;
}
