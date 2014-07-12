#!/usr/bin/perl

#my $all_output_file = 'ross.dat';
my @models = ("wifi", "disksim", "raid");
my @model_opts = ("", "--end=10000000", "");
my @model_lp_prefix = ("--nlp=", "--nlp=", "--nfs=");
my @model_lp_targets = (96, 768, 384);
my @processes = (2, 4, 8, 12, 16);
my @btls = ("tcp");
my $iterations = 10;
my $ross_path = "~/ross-build/ross/models";
my @memory = (15360, 15360, 7680, 5120, 3840);

my $i = 0;
for my $model (@models) {
  my $output_file = $model . '.dat';
  open(OUT, ">$output_file") || die "Could not open $output_file\n";
  my $np_index = 0;
  for my $np (@processes) {
    my $num_lp = $model_lp_targets[$i] / $np;
    for my $btl (@btls) {
      print OUT "$np Processes\t$btl";
      for (my $iter = 0; $iter < $iterations; $iter++) {
        print "Running $model with $np processes over $btl\n";
        my $output = `mpirun -n $np -host curls,peter --mca btl $btl,self $ross_path/$model/$model --synch=3 $model_opts[$i] $model_lp_prefix[$i]$num_lp --memory=$memory[$np_index]`;
        my @lines = split("\n", $output);
        for (@lines) {
          if ( /Running Time = ([0-9.]+) seconds/ ) {
            print OUT "\t$1";
          }
        }
      }
      print OUT "\n";
    }
    ++$np_index;
  }
  ++$i;
}
