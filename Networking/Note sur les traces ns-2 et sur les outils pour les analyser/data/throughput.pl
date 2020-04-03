$infile=$ARGV[0];
$fromnode=$ARGV[1];
$tonode=$ARGV[2];
$protocol=$ARGV[3];

#we compute how many bytes were transmitted during time interval specified by granularity in ms
$sum=0;
$clock=0;
$count=1;
$cumul=0;
# Mean over 1 second
$granularity=1;

open (DATA, "<$infile")
	|| die "Can't open $infile: $!";
	
	while (<DATA>) {
		@x = split(' ');	
		if ( $x[1]-$clock <= $granularity)
		{
			if ($x[0] eq 'r')
			{
				if ($x[2] eq $fromnode)
				{
					if ($x[3] eq $tonode)
					{
						if ($x[4] eq $protocol)		
						{
							$sum=$sum+$x[5];
						}
					}
				}
			}
		}	
		else
		{
			$throughput = (($sum*8)/$granularity)/1000000;
			$cumul = $cumul + $throughput;
			$cumul_average = $cumul / $count;
      $count ++;	
			print STDOUT "$x[1] $throughput $cumul_average\n";
			$clock=$clock+$granularity;
			$sum=0;
		}
	}
	close DATA;
	exit (0);
		
