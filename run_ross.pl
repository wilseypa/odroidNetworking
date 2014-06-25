#!/usr/bin/perl

#my $all_output_file = 'ross.dat';
my @models = ("wifi", "disksim", "raid", "qhold_fp", "airport");
my @model_opts = ("", "", "--end=1000000 --nfs=16", "", "--nplanes=64");
my @processes = (2, 4, 8, 12, 16);
my @btls = ("tcp" , "openib");
my $iterations = 2;
my $ross_path = "/root/ross-build/ross/models";

my $i = 0;
for my $model (@models) {
  my $output_file = $model . '.dat';
  open(OUT, ">$output_file") || die "Could not open $output_file\n";

  for my $np (@processes) {
    for my $btl (@btls) {
      print OUT "$np Processes\t$btl";
      for (my $iter = 0; $iter < $iterations; $iter++) {
        my $output = `mpirun -n $np -host xu0,xu1,xu2,xu3 --mca btl $btl,self,sm $ross_path/$model/$model --synch=3 $model_opts[$i]`;
        my @lines = split("\n", $output);
        for (@lines) {
          if ( /Running Time = ([0-9.]+) seconds/ ) {
            print OUT "\t$1";
          }
        }
      }
      print OUT "\n";
    }
  }
  ++$i;
}
