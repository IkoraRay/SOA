/*******************************************************************************
* Este programa esta baseado no segundo experimento do curso sobre tempo real 
* do Laboratorio Embry-Riddle
* 
* Seguem os comentarios originais:
*
* Experiment #2: Multi-Tasking, Measuring Drift
*
*    Programmer: Eric Sorton
*          Date: 1/27/97
*           For: MSE599, Special Topics Class
*
*       Purpose: When a basic sleep call is used to determine the precise time
*                when an action will occur the problem of drift occurs. 
*                The measurement of time is imprecise.  Similarly, the time in 
*                which the sleep call returns is imprecise.  Over time, this 
*                will cause the ocurrence of time to drift.  Just as if a clock 
*                loses 1 second every day, over one day, it is significant, but 
*                over a year, it loses 365 seconds, which is over 6 minutes.  
*                This is an example of drift.
*
*       Proposito: Quando uma chamada b�sica sleep e usada para determinar o
*                instante exato em que alguma acao vai ocorrer, ocorre o problema
*                do desvio. A medicao de tempo e imprecisa. Similarmente, o tempo
*                que demora o retorno da chamada sleep tambem e impreciso. Ao
*                longo do tempo, isto ocasionara um desvio de tempo. Algo como se
*                um relogio perdesse um segundo a cada dia. Ao longo de um dia, 
*                essa diferenca e insignificante, mas, ao longo de um ano, sao 
*                perdidos 365 segundos, o que e superior a 6 minutos. Este e um
*                exemplo de desvio.
*
*******************************************************************************/

/*
 * Includes Necessarios, verifique se as bibliotecas no diretorio sys/ estao
 * lah. Caso nao estejam, verifique onde estao e altere o include
 */

#include <sys/time.h>		/* for gettimeofday() */
#include <unistd.h>		/* for gettimeofday() and fork() */
#include <stdio.h>		/* for printf() */
#include <sys/types.h>		/* for wait() */
#include <sys/wait.h>		/* for wait() */
#include <stdlib.h>
/*
 * Pergunta 1: o que o compilador gcc faz com o arquivo .h, cujo nome aparece ap�s o include?
 */
  /*Resposta: Quando o compilador encontra um comando #include, ele procura a biblioteca indicada
  no diretório, e vincula a biblioteca ao programa

/*
 * Pergunta 2: apresentar (parcialmente) e explicar o que h� em <stdio.h>
 * Resposta: Na biblioteca stdio.h contém macros referentes à entrada e saída de dados, e arquivos.
 * Exemplo:
 * int      feof(FILE *);
	int      ferror(FILE *);
	int      fflush(FILE *);
	int      fgetc(FILE *);
	int      fgetpos(FILE *, fpos_t *);
	char    *fgets(char *, int, FILE *);
	int      fileno(FILE *);
	void     flockfile(FILE *);
	FILE    *fopen(const char *, const char *);
	int      fprintf(FILE *, const char *, ...);
	int      fputc(int, FILE *);
	int      fputs(const char *, FILE *);
	size_t   fread(void *, size_t, size_t, FILE *);
	FILE    *freopen(const char *, const char *, FILE *);
	int      fscanf(FILE *, const char *, ...);
	int      fseek(FILE *, long int, int);
	int      fseeko(FILE *, off_t, int);
	int      fsetpos(FILE *, const fpos_t *);
	long int ftell(FILE *);
	off_t    ftello(FILE *);
	int      ftrylockfile(FILE *);
	void     funlockfile(FILE *);
	size_t   fwrite(const void *, size_t, size_t, FILE *);
	int      getc(FILE *);
	int      getchar(void);
 */

/*
 * Pergunta 3: qual eh a funcao da diretiva include (linha que come�a com #), com rela��o ao compilador?
 * Resposta:A função include é utilizada para que o compilador inclua
 *  um arquivo e/ou biblioteca externa no programa
 */

/*
 * NO_OF_ITERATIONS e o numero de vezes que vai se repetir o loop existente
 * em cada futuro processo filho. 
 */



/*
 * NO_OF_CHILDREN eh o numero de filhos a serem criados, cada qual responsavel
 * pela medida do desvio. 
 */

#define NO_OF_CHILDREN	5

/*
 * SLEEP_TIME corresponde a quantidade de tempo para ficar bloqueado.
 */

#define SLEEP_TIME 400

/*
 * MICRO_PER_SECOND define o numero de microsegundos em um segundo
 */


/*
 * Programa Principal. Contem tanto o codigo para o processo pai como
 * o codigo dos futuros filhos
 */

int main( int argc, char *argv[] )
{
/*
 * Pergunta 4: o que s�o e para que servem argc e argv? N�o esqueca de
 * considerar o * antes de argv.
 * Resposta: Os parametros argc e argv servem para dar ao programador acesso a linha de comando
 * com a qual o programa foi chamado
 */
	/*
	 * start_time e stop_time conterao o valor de tempo antes e depois
	 * que as trocas de contexto comecem
         */

      /*
       * Outras variaveis importantes
       */

   	char buffer[6];
	sprintf(buffer,"%d",SLEEP_TIME);

      int count;
	int child_no;
	char *args[]={"./filho",buffer}; 

	/*
	 * Criacao dos processos filhos
	 */

	int rtn = 1;
	for( count = 0; count < NO_OF_CHILDREN; count++ ) {
		if( rtn != 0 ) {
			rtn = fork();			
		} else {
			break;
		}
	}


	/*
	 * Verifica-se rtn para determinar se o processo eh pai ou filho
	 */
		printf("Passou aqui1");
	if( rtn == 0 ) {
		printf("Passou aqui2");
		 execvp(args[0],args);
	
	} else {
		/*
		 * Sou pai, aguardo o termino dos filhos
		 */
		for( count = 0; count < NO_OF_CHILDREN; count++ ) {
			wait(NULL);
		}
	}

	exit(0);
}
