#!/usr/bin/perl -w

use strict;
use warnings;

my $mode = shift || 'server';
my $size = 2;
my $output_file = 'lat_tests.csv'

my $iterations = 1000;

@tests = ('ib_send_lat', 'ib_write_lat', 'ib_read_lat');
@connection_types = ('RC', 'UC', 'UD');
@options = ('', '-z', '-R');

my $target;
my $server;

if ( $mode =~ /server|target|slave/ ) {
  $target = '';
  $server = 1;
}
else if ( $mode =~ /client|initiator|master/ ) {
  $target = ' ' . shift(@ARGV) . ' -U';
  $server = 0;
}
else {
  die "Invalid server/client specifier\n";
}

open(OUT, ">$output_file" . "_$size") || die "Could not open $output_file for writing\n";

for $test (@tests) {
  for $option (@options) {
    for $type (@connection_types) {
      if ( !$server ) {
        sleep 1;
      }
      my $output = `$test -c $type $option -n $iterations -s $size $target`;
      my @lines = split("\n", $output);
      for (@lines) {
        if ( /^([0-9]+), ([0-9.]+)$/ ) {
          print OUT $2 . ',';
