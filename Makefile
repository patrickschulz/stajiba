default:
	gcc -g -O0 main.c vector.c noise.c engineering.c export.c transfer.c parameter.c pll.c -lm
	#gcc -g -O0 simulated_annealing.c -lm
