#include <sys/time.h>		/* for gettimeofday() */
#include <unistd.h>		/* for gettimeofday() and fork() */
#include <stdio.h>		/* for printf() */
#include <sys/types.h>		/* for wait() */
#include <sys/wait.h>		/* for wait() */
#include <stdlib.h>

#define NO_OF_ITERATIONS	1000
#define MICRO_PER_SECOND	1000000

int main( int argc, char *argv[] )
{
	int SLEEP_TIME;
	SLEEP_TIME = atoi(argv[1]);

	struct timeval start_time;
	struct timeval stop_time;
	   float drift;


		gettimeofday( &start_time, NULL );

	
		/*
		 * Este loop ocasiona a minha dormencia, de acordo com
		 * SLEEP_TIME, tantas vezes quanto NO_OF_ITERATIONS
		 */	
		
			usleep(SLEEP_TIME);

		/*
		 * Paraobter o tempo final
		 */
		gettimeofday( &stop_time, NULL );

		/*
		 * Calcula-se o desvio
		 */
		drift = (float)(stop_time.tv_sec  - start_time.tv_sec);
		drift += (stop_time.tv_usec - start_time.tv_usec)/(float)MICRO_PER_SECOND;

		/*
		 * Exibe os resultados
		 */
		printf("%d\n",SLEEP_TIME);
		printf("Filho -- PID: %d -- desvio total: %.8f -- desvio medio: %.8f\n",
			getpid(), drift - NO_OF_ITERATIONS*SLEEP_TIME/MICRO_PER_SECOND,
			(drift - NO_OF_ITERATIONS*SLEEP_TIME/MICRO_PER_SECOND)/NO_OF_ITERATIONS);
		
		/*
		 * Pergunta 5: Qual a rela��o: entre SLEEP_TIME e o desvio, nenhuma, direta 
		 * ou indiretamente proporcional? 
		 */
	
}

