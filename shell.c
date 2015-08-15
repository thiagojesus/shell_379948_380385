#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX 256

void read_line(char str[], int n){ /* le a string corretamente */
  int ch, i = 0;

  while ((ch = getchar()) != '\n')
    if (i < n)
    str[i++] = ch;
    str[i] = '\0';
}

int parse(char *line, char **argv) /*funcao que separa o comando dos argumentos e identifica os caracteres &, < e >*/
{
    int background=0,i=0,f1,f2=0;
    char in[MAX], out[MAX];
     while (*line != '\0') {       /* nao chegou no fim da linha */ 
          while (*line == ' ' || *line == '\t' || *line == '\n')
               *line++ = '\0';     /* substitui espacos com \0 para que o vetor de argumentos saiba onde termina o argumento    */
               if(*line == '>'){ /*redirecionamento de saida*/
                   i = 0;
		   line++;
		   memset(out,0,MAX);/*inicializa a variavel out*/
                   while(*line!=' ' && *line!='\0'){ /*obtem o destino para onde a saida sera redirecionada*/
                       out[i]=*line;
                       i++;
		       line++;
                   }
                   f1 = open(out, O_WRONLY | O_TRUNC | O_CREAT, 0644); /*chamada de sistema para abrir o arquivo*/
                   dup2(f1, 1); /*redireciona a saida do processo para o que esta guardado em f1*/
                   close(f1); 
		   if(*line == '\0'){
		       *argv = '\0';
		       return 0;
                   }
               }else{
		   if(*line == '<'){ /*redirecionamento de entrada*/
                   i = 0;
		   line++;
		   memset(in,0,MAX);/*inicializa a variavel in*/
                   while(*line!=' ' && *line!='\0'){ /*obtem o destino para onde a entrada sera redirecionada*/
                       in[i]=*line;
                       i++;
		       line++;
                   }
                   f2 = open(in, O_RDONLY); /*chamada de sistema para abrir o arquivo*/
                   dup2(f2, 0); /*redireciona a entrada do processo para o que esta guardado em f2*/
                   close(f2);
		   if(*line == '\0'){
		       *argv = '\0';
		       return 0;
                   }
                   }else{
		       if(*line == '&'){
		          background = 1;
                       }else{
			  *argv++ = line;          /* salva a posicao do argumento     */
                       }
		       
                     
                   }
               }
          while (*line != '\0' && *line != ' ' && 
                 *line != '\t' && *line != '\n') {        
                        line++;      /* pula o argumento    */
                 }
                          
     }
     *argv = '\0';                 /* marca o fim da lista de argumentos  */
     return background;
}

void  execute(char **argv, int background)
{
     pid_t  pid;
     int    status;

     if ((pid = fork()) < 0) {     /* cria o processo filho           */
          printf("Erro no Fork\n");
          exit(1);
     }
     else if (pid == 0) {          /* para o processo filho         */
          if (execvp(*argv, argv) < 0) {     /* executa o comando  */
               printf("Comando Invalido\n");
               exit(1);
          }
     }
     else { 
	  if(background == 0){    //se for 0 nao sera executado em segundo plano, caso contrario nao espera o pid                 
	       while (wait(&status) != pid);       /* espera terminar  */
               
          }else{
	  }                                 
          
     }
     
}


int main() {
  char comando[MAX];
  char *argv[MAX];
  int background=0, in, out;
  in = dup(STDIN_FILENO); /*guarda a entrada padrao*/
  out = dup(STDOUT_FILENO); /*guarda a saida padrao*/
  while (1) {
    fprintf(stdout,"> "); /*so para garantir que sera realmente exibido na saida padrao rs*/
    read_line(comando, MAX);
    if (!strcmp(comando, "exit")) {
      exit(EXIT_SUCCESS);
    }
      background = parse(comando,argv);
      execute(argv, background);
      dup2(in,0); /*volta a entrada padrao*/
      dup2(out,1); /*volta a saida padrao*/
    }
  }

