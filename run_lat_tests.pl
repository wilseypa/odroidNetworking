#!/usr/bin/perl -w

use strict;
use warnings;

my $mode = shift || 'server';
my $size = 2;
my $output_file = 'lat_tests.csv';

my $iterations = 1000;

my @tests = ('ib_send_lat', 'ib_write_lat', 'ib_read_lat');
my @connection_types = ('RC', 'UC', 'UD');
my @options = ('', '-z', '-R');

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
}

for my $test (@tests) {
  for my $option (@options) {
    my $opt_string;
    if ( $option eq '' ) {
      $opt_string = 'IB Connection';
    }
    elsif ( $option eq '-z' ) {
      $opt_string = 'RDMA CM Connection with IB QPs';
    }
    elsif ( $option eq '-R' ) {
      $opt_string = 'RDMA CM QPs';
    }
    for my $type (@connection_types) {
      if ( !$server ) {
        sleep 1;
      }
      my $output = `$test -c $type $option -n $iterations -s $size $target`;
      if ( !$server ) {
        print OUT "$test ${size}B $type $opt_string,";
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

if ( !$server ) {
  close OUT;
}
