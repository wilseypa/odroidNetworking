#!/usr/bin/perl

my $output_file = 'imb.dat';
my @btls = ("tcp" , "openib");
my @processes = (4 , 8, 12, 16);
my $imb_path = "/root/imb/3.2.4/src/IMB-MPI1";
my $msglog = 16;

open(OUT, ">$output_file") || die "Could not open $output_file\n";

for my $btl (@btls) {
  my $transport;
  if ( $btl eq "tcp" ) {
    $transport = "TCP";
  }
  else {
    $transport = "RoCE";
  }
  for my $np (@processes) {
    my $output = `mpirun -n $np -host xu0,xu1,xu2,xu3 --mca btl $btl,self,sm $imb_path -msglog $msglog -multi 1 pingpong`;
    my @lines = split("\n", $output);
    my $group = 0;
    my $group_max = $np/2-1;
    my $group_sum_lat = 0;
    my $group_sum_bw = 0;
    for (@lines) {
      if ( /bytes/ && /repetitions/ ) {
        print OUT "Transport\tProcesses\tMessage size (B)\tRepetitions\tLatency (microseconds)\tBandwidth (MB/s)\n";
      }
      if ( $_ !~ /^[ \t]*#/ && /[0-9]/ ) {
        s/^\s+//;
        my @numbers = split /\s+/;
        $group_sum_lat += $numbers[3];
        $group_sum_bw += $numbers[4];
        if ( ++$group >= $group_max ) {
          my $mean_lat = $group_sum_lat / $np;
          my $mean_bw = $group_sum_bw / $np;
          print OUT "$transport\t$np\t$numbers[1]\t$numbers[2]\t$mean_lat\t$mean_bw\n";
          $group_sum_lat = 0;
          $group_sum_bw = 0;
          $group = 0;
        }
      }
    }
  }
}
