#!/usr/bin/perl

my $output_file = 'netpipe.dat';
my @btls = ("openib");
my $max_size = 2 ** 16;

my $netpipe_path = "/root/NetPIPE-3.7.1/NPmpi";

open(OUT, ">$output_file") || die "Could not open $output_file\n";

for my $btl (@btls) {
  my $output = `mpirun -n 2 -host xu0,xu2 --mca btl $btl,self,sm $netpipe_path -u $max_size`;
  my @lines = split("\n", $output);
  my @sizes;
  my @latencies;
  my @bandwidths;
  my $i = 0;
  for (@lines) {
    if ( /[0-9]+:\s+([0-9]+)\s+bytes\s+([0-9]+)\s*times\s*-->\s*([0-9.]+) Mbps in\s*([0-9.]+) usec/ ) {
      $sizes[$i] = $1;
      $bandwidths[$i] = $3;
      $latencies[$i] = $4;
      ++$i;
    }
  }
  print OUT $btl . "\n";
  print OUT "Message Size (B)\t";
  for (@sizes) {
    print OUT;
    print OUT "\t";
  }
  print OUT "\n";
  print OUT "Bandwidth (MB/s)\t";
  for (@bandwidths) {
    print OUT;
    print OUT "\t";
  }
  print OUT "\n";
  print OUT "Latency (microseconds)\t";
  for (@latencies) {
    print OUT;
    print OUT "\t";
  }
  print OUT "\n\n";
}
