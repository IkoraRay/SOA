/* Includes Necessarios */
#include <errno.h>              
#include <sys/time.h>           
#include <stdio.h>              
#include <unistd.h>             
#include <sys/types.h>          
#include <sys/wait.h>           
#include <signal.h>          
#include <sys/ipc.h>            
#include <sys/shm.h>            
#include <sys/sem.h>            
#include <stdlib.h> 		/* Include adiconado */

/* Constantes Necessarias */
#define SEM_KEY		0x1243
#define SHM_KEY		0x1432
#define NO_OF_CHILDREN	3

#define PROTECT


int	g_sem_id;
int	g_shm_id;
int	*g_shm_addr;

struct sembuf	g_sem_op1[1];
struct sembuf	g_sem_op2[1];

/* Vetor de caracteres */
char g_letters_and_numbers[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz 1234567890";

/* Funcoes */
void PrintChars( void );

int main( int argc, char *argv[] )
{
	int rtn;
	int count;
        
	int pid[NO_OF_CHILDREN];

	/* Construindo a estrutura de controle do semaforo  */
	g_sem_op1[0].sem_num   =  0;
	g_sem_op1[0].sem_op    = -1;
	g_sem_op1[0].sem_flg   =  0;

	g_sem_op2[0].sem_num =  0;
	g_sem_op2[0].sem_op  =  1;
	g_sem_op2[0].sem_flg =  0;

	/* Criando o semaforo */	
	if( ( g_sem_id = semget( SEM_KEY, 1, IPC_CREAT | 0666 ) ) == -1 ) {
		fprintf(stderr,"chamada a semget() falhou, impossivel criar o conjunto de semaforos!");
		exit(1);
	}
	
	if( semop( g_sem_id, g_sem_op2, 1 ) == -1 ) {
		fprintf(stderr,"chamada semop() falhou, impossivel inicializar o semaforo!");
		exit(1);
	}

	/* Criando o segmento de memoria compartilhada */
	if( (g_shm_id = shmget( SHM_KEY, sizeof(int), IPC_CREAT | 0666)) == -1 ) {
		fprintf(stderr,"Impossivel criar o segmento de memoria compartilhada!\n");
		exit(1);
	}
	if( (g_shm_addr = (int *)shmat(g_shm_id, NULL, 0)) == (int *)-1 ) {
		fprintf(stderr,"Impossivel associar o segmento de memoria compartilhada!\n");
		exit(1);
	}
	*g_shm_addr = 0;

	rtn = 1;
	for( count = 0; count < NO_OF_CHILDREN; count++ ) {
		if( rtn != 0 ) {
			rtn = fork();
			pid[count] = rtn;
	    } else {
			break;
	    }
    }

	if( rtn == 0 ) {

		printf("Filho %d comecou ...\n", count);

		PrintChars();

    } else {
		usleep(15000);

		/* Matando os filhos */
		kill(pid[0], SIGKILL);
		kill(pid[1], SIGKILL);
		kill(pid[2], SIGKILL);

		/* Removendo a memoria compartilhada */
		if( shmctl(g_shm_id,IPC_RMID,NULL) != 0 ) {
			fprintf(stderr,"Impossivel remover o segmento de memoria compartilhada!\n");
			exit(1);
		}

		/* Removendo o semaforo */
		if( semctl( g_sem_id, 0, IPC_RMID, 0) != 0 ) {
			fprintf(stderr,"Impossivel remover o conjunto de semaforos!\n");
			exit(1);
		}

			exit(0);
    }
}


void PrintChars( void )
{
	struct timeval tv;
	int number;

	int tmp_index;
	int i;

	/* Este tempo permite que todos os filhos sejam inciados */
	usleep(200);

	while(1) {

		if( gettimeofday( &tv, NULL ) == -1 ) {
			fprintf(stderr,"Impossivel conseguir o tempo atual, terminando.\n");
			exit(1);
		}
		number = ((tv.tv_usec / 47) % 3) + 1;

		#ifdef PROTECT
			if( semop( g_sem_id, g_sem_op1, 1 ) == -1 ) {
				fprintf(stderr,"chamada semop() falhou, impossivel fechar o recurso!");
				exit(1);
			}
		#endif

		/* Lendo o indice do segmento de memoria compartilhada */
		tmp_index = *g_shm_addr;

		for( i = 0; i < number; i++ ) {
			if( ! (tmp_index + i > sizeof(g_letters_and_numbers)) ) {
				fprintf(stderr,"%c", g_letters_and_numbers[tmp_index + i]);
				usleep(1);
			}
		}

		/* Atualizando o indice na memoria compartilhada */
		*g_shm_addr = tmp_index + i;

		if( tmp_index + i > sizeof(g_letters_and_numbers) ) {
			fprintf(stderr,"\n");
			*g_shm_addr = 0;
		}

		/* Liberando o recurso se a macro PROTECT estiver definida */

		#ifdef PROTECT
			if( semop( g_sem_id, g_sem_op2, 1 ) == -1 ) {      		
				fprintf(stderr,"chamada semop() falhou, impossivel liberar o recurso!");
				exit(1);
			}
		#endif

	}
}