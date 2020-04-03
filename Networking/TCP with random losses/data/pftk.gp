reset;
set title "Comparaison des d\351bits th\351oriques et simul\351s de TCP newreno"; 
set terminal postscript eps color "Helvetica" 20;
set encoding iso_8859_1;
set output 'debit_newreno_theo_vs_sim.eps';

set xlabel "Taux de pertes";
set ylabel "D\351bit en Mbits";
set key center left;

set xrange [0.00001:1]
set logscale
#set yrange [0:1]

rtt=0.04
w(p)=1+sqrt( (8*(1-p)/(3*p)) + 1 )
Q(h)=3./h
f(t)=1+t+2*t*t+4*t*t*t+8*t*t*t*t+16*t*t*t*t*t+32*t*t*t*t*t*t
espX(t)=0.5+sqrt( (2*(1-t)/(3*t)) + 0.5*0.5 )
dfull(x)= 8*1460./1000000*( ((1-x)/x) + w(x) + Q(w(x))/(1-x) ) / ( rtt*(1+espX(x)) + Q(w(x))*4*rtt*f(x)/(1-x) )

d1(x) = (sqrt(3./2)*1460*8/(rtt*sqrt(x)))/1000000
d2(x) = (1460*8/(0.5*sqrt(2*x/3)+ 0.5*3*sqrt(3*x/8)*x*(1+32*x*x)))/1000000



#'debit_fct_p.tr' u 1:($2) w l t "Simulation", \
#'debit_fct_p.tr' u 1:($2) w p notitle, \
#set xtics (0.000001,0.000005,0.00001,0.00005,0.0001,0.0005,0.001)
plot \
d1(x) w l t "Modele Approxim\351" ,\
dfull(x) w l t "Modele Complet" 
	
