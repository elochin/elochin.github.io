set ns [new Simulator]

set ftrace [open fout.tr w]
set nf [open out.nam w]
$ns namtrace-all $nf
set f [open out.tr w] 
$ns trace-all $f

proc finish {} {
	global ns ftrace nf f
	$ns flush-trace
	close $nf
	close $f
	close $ftrace
	exit 0
}

proc windowtrace {} {
global tcp0 ftrace
set ns [Simulator instance]
set time 0.01
set now [$ns now]
puts $ftrace "$now [$tcp0 set cwnd_]"
$ns at [expr $now+$time] "windowtrace"
}

Agent/TCP set packetSize_ 	1500
Agent/TCP set maxcwnd_		30 ;# valeur max de la cong. window. Une valeur à 0 => max == infini
Agent/TCP set window_		30 ;# advertised ou receiver window rwnd
Agent/TCP set windowInit_ 1
Agent/TCPSink/DelAck set interval_ 0.00001

set n0 [$ns node]
set tcp0 [new Agent/TCP]
$ns attach-agent $n0 $tcp0
set ftp [new Application/FTP]
$ftp attach-agent $tcp0

set n1 [$ns node]		
set tcp1 [new Agent/TCPSink/DelAck]
$ns attach-agent $n1 $tcp1

$ns duplex-link $n0 $n1 10Mb 10ms DropTail

$ns connect $tcp0 $tcp1

$ns at 0.0 "$ftp start"
$ns at 10.0 "$ftp stop"
$ns at 10.1 "finish"
$ns at 0.0 "windowtrace"

$ns run
