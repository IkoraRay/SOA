
#define PROTECT

#include <errno.h>              /* errno and error codes */
#include <sys/time.h>           /* for gettimeofday() */
#include <stdio.h>              /* for printf() */
#include <unistd.h>             /* for fork() */
#include <sys/types.h>          /* for wait() */
#include <sys/wait.h>           /* for wait() */
#include <signal.h>             /* for kill(), sigsuspend(), others */
#include <sys/ipc.h>            /* for all IPC function calls */
#include <sys/shm.h>            /* for shmget(), shmat(), shmctl() */
#include <sys/sem.h>            /* for semget(), semop(), semctl() */
#include <stdlib.h>
#include <sys/msg.h>
/*
 * Constantes Necessarias 
 */
#define SEM_KEY		0x1243
#define SHM_KEY		0x1432
#define NO_OF_CHILDREN	8

#define MESSAGE_QUEUE_ID1	3102
#define SENDER_DELAY_TIME	10



/*
 * As seguintes variaveis globais contem informacao importante. A variavel
 * g_sem_id e g_shm_id contem as identificacoes IPC para o semaforo e para
 * o segmento de memoria compartilhada que sao usados pelo programa. A variavel
 * g_shm_addr e um ponteiro inteiro que aponta para o segmento de memoria
 * compartilhada que contera o indice inteiro da matriz de caracteres que contem
 * o alfabeto que sera exibido.
*/
int	g_sem_id;
int	g_shm_id;
int	*g_shm_addr;


/*
 * As seguintes duas estruturas contem a informacao necessaria para controlar
 * semaforos em relacao a "fecharem", se nao permitem acesso, ou 
 * "abrirem", se permitirem acesso. As estruturas sao incializadas ao inicio
 * do programa principal e usadas na rotina PrintAlphabet(). Como elas sao
 * inicializadas no programa principal, antes da criacao dos processos filhos,
 * elas podem ser usadas nesses processos sem a necessidade de nova associacao
 * ou mudancas.
*/
struct sembuf	g_sem_op1[1];
struct sembuf	g_sem_op2[1];

/*
 * O seguinte vetor de caracteres contem o alfabeto que constituira o string
 * que sera exibido.
*/
char g_letters_and_numbers[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz 1234567890";

/*
 * Funcoes
 */
void SendChars(int queue_id );

void RecieveChars (int queue_id);

int criaFilaMensagem(key_t key);

void excluirFilaMensagem(int queue_id);

/*
 * Programa Principal
 */
int main( int argc, char *argv[] )
{
	
      /*
       * Variaveis necessarias
       */
        int rtn;
        int count;
    	int queue_id; 
    
        int pid[NO_OF_CHILDREN];

	
	g_sem_op1[0].sem_num   =  0;
	g_sem_op1[0].sem_op    = -1;
	g_sem_op1[0].sem_flg   =  0;

	/* 
	 * Pergunta 1: Se usada a estrutura g#include <sys/msg.h>_sem_op1 ter� qual efeito em um conjunto de sem�foros?
	 */

	g_sem_op2[0].sem_num =  0;
	g_sem_op2[0].sem_op  =  1;
	g_sem_op2[0].sem_flg =  0;
	
	/*
	 *Criando a fila de mensagens
	 */
	queue_id = criaFilaMensagem(MESSAGE_QUEUE_ID1);
	/*
	 * Criando o semaforo
	 */	
	if( ( g_sem_id = semget( SEM_KEY, 1, IPC_CREAT | 0666 ) ) == -1 ) {
		fprintf(stderr,"chamada a semget() falhou, impossivel criar o conjunto de semaforos!");
		exit(1);
	}
	
	if( semop( g_sem_id, g_sem_op2, 1 ) == -1 ) {
		fprintf(stderr,"chamada semop() falhou, impossivel inicializar o semaforo!");
		exit(1);
	}

	/* 
	 * Pergunta 2: Para que serve esta operacao semop(), se n�o est� na sa�da de uma regi�o cr�tica?
	 */

	/*
	 * Criando o segmento de memoria compartilhada
	 */
	if( (g_shm_id = shmget( SHM_KEY, sizeof(int), IPC_CREAT | 0666)) == -1 ) {
		fprintf(stderr,"Impossivel criar o segmento de memoria compartilhada!\n");
		exit(1);
	}
	if( (g_shm_addr = (int *)shmat(g_shm_id,NULL, 0)) == (int *)-1 ) {
		fprintf(stderr,"Impossivel associar o segmento de memoria compartilhada!\n");
		exit(1);
	}
	*g_shm_addr = 0;

	/*
	 * Pergunta 3: Para que serve essa inicializa��o da mem�ria compartilhada com zero?
	 */

       /*
        * Criando os filhos
        */
	  
       rtn = 1;
       for( count = 0; count < NO_OF_CHILDREN; count++ ) {
               if( rtn != 0 ) {
                       pid[count] = rtn = fork();
               } else {
                       break;
               }
       }

       /*
        * Verificando o valor retornado para determinar se o processo e 
        * pai ou filho 
        */
       if( rtn == 0 ) {
                /*
                 * Eu sou um filho
                 */
                printf("Filho %i comecou ...\n", count);
				if(count < 4)
					SendChars(queue_id);
				else
				{
					RecieveChars(queue_id);	
				}
				

        } else {
                usleep(15000);

                /*
                 * Matando os filhos 
                 */
                kill(pid[0], SIGKILL);
                kill(pid[1], SIGKILL);
                kill(pid[2], SIGKILL);
                kill(pid[3], SIGKILL);
				kill(pid[4], SIGKILL);	
				kill(pid[5], SIGKILL);
				kill(pid[6], SIGKILL);
				kill(pid[7], SIGKILL);
                /*
                 * Removendo a memoria compartilhada
                 */
                if( shmctl(g_shm_id,IPC_RMID,NULL) != 0 ) {
                        fprintf(stderr,"Impossivel remover o segmento de memoria compartilhada!\n");
                        exit(1);
                }

                /*
                 * Removendo o semaforo
                 */
                if( semctl( g_sem_id, 0, IPC_RMID, 0) != 0 ) {
                        fprintf(stderr,"Impossivel remover o conjunto de semaforos!\n");
                        exit(1);
                }
				/*
				*Removendo a fila de mensagens
				*/
				excluirFilaMensagem(queue_id);

                exit(0);
        }
}



void SendChars(int queue_id )
{
	struct timeval tv;
	int number;

	int tmp_index;
	int i;

	/*
	 * Este tempo permite que todos os filhos sejam inciados
	 */
	usleep(200);

	/*
	 * Entrando no loop principal
	 */
	while(1) {

		/*
                 * Conseguindo o tempo corrente, os microsegundos desse tempo
             	 * sao usados como um numero pseudo-randomico. Em seguida,
            	 * calcula o numero randomico atraves de um algoritmo simples
		 */
		if( gettimeofday( &tv, NULL ) == -1 ) {
			fprintf(stderr,"Impossivel conseguir o tempo atual, terminando.\n");
			exit(1);
		}
		number = ((tv.tv_usec / 47) % 3) + 1;

		/*
		 * Pergunta 5: quais os valores poss�veis de serem atribuidos 
		 * a number?
		 */

		/*
		 * O #ifdef PROTECT inclui este pedaco de codigo se a macro
            * PROTECT estiver definida. Para sua definicao, retire o comentario
            * que a acompanha. semop() e chamada para fechar o semaforo.
            */

		//#ifdef PROTECT
		if( semop( g_sem_id, g_sem_op1, 1 ) == -1 ) {
                       	fprintf(stderr,"chamada semop() falhou, impossivel fechar o recurso!");
                       	exit(1);
               	 }
		//#endif

		/*
		 * Lendo o indice do segmento de memoria compartilhada
		 */
		tmp_index = *g_shm_addr;


		/*
            * Repita o numero especificado de vezes, esteja certo de nao
            * ultrapassar os limites do vetor, o comando if garante isso
		 */
		for( i = 0; i < number; i++ ) {

			//if( ! (tmp_index + i > sizeof(g_letters_and_numbers)) ) {
			//	fprintf(stderr,"%c", g_letters_and_numbers[tmp_index + i]);
			//	usleep(1);
			//}
			if(msgsnd(queue_id,g_letters_and_numbers,sizeof(char),0) == -1 ) {
			fprintf(stderr, "Impossivel enviar mensagem!\n");
			exit(1);
		}
		}

		/*
		 * Atualizando o indice na memoria compartilhada
		 */

		*g_shm_addr = tmp_index + i;

		/*
         	 * Se o indice e maior que o tamanho do alfabeto, exibe um
         	 * caractere return para iniciar a linha seguinte e coloca
         	 * zero no indice
		 */
		if( tmp_index + i > sizeof(g_letters_and_numbers) ) {
			fprintf(stderr,"\n");
			*g_shm_addr = 0;
		}

		/*
		 * Liberando o recurso se a macro PROTECT estiver definida
		 */

		//#ifdef PROTECT
		if( semop( g_sem_id, g_sem_op2, 1 ) == -1 ) {      		
                        fprintf(stderr,"chamada semop() falhou, impossivel liberar o recurso!");
                       	exit(1);
               	}
		//#endif		   

	}
}
void RecieveChars(int queue_id)
{

}
int criaFilaMensagem(key_t key)	{
	int queue_id;
        
	if( (queue_id = msgget(key, IPC_CREAT | 0666)) == -1 ) {
		fprintf(stderr,"Impossivel criar a fila de mensagens!\n");
		exit(1);
	}

	return queue_id;
}
void excluirFilaMensagem(int queue_id)	{

   if( msgctl(queue_id,IPC_RMID,NULL) != 0 ) {
		fprintf(stderr,"Impossivel remover a fila!\n");
			exit(1);
	}
}

