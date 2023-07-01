#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> //%*c
#define CAD "dados.bin"

typedef struct{
  char senha[30];
  int num;
  char nome[40];
  double saldo;
} tp_cliente;

void code (char* );
void decode (char* );
int login(tp_cliente*, int);
void grava_arquivo (FILE*, tp_cliente*, int);
void le_arquivo (FILE*, tp_cliente*, int);
void le_numero(FILE*, int*);
void cadastro (tp_cliente*, int*);
void lista_cliente (tp_cliente*, int);

void pausa(){ //funcao de espera
  char wait[5];
  
    fflush(stdin);
    fgets(wait,3,stdin);
}

int main(void) 
{
  FILE* arq;
  int opc, posi, sair=1, programa=1, nreg = 0;      
  tp_cliente *cliente;     //struct para vetor de cliente alocada dinamicamente   
  double quantidade = 0;
  int volta=0;
  int u; //CASE 3
  char r = ' ';//CASE 3

  le_numero(arq, &nreg);
  
  cliente = (tp_cliente*) calloc (nreg, sizeof(tp_cliente));
  
  le_arquivo(arq, cliente, nreg);

   printf ("\n\n LINHA 48 DEBUG: !!!! %s %s\n\n", cliente[0].nome, cliente[0].senha);//DEBUG
  
    do{
      printf("Bem vindo ao banco secsafe!\n");
      printf("[1]Login\n");
      printf("[2]cadastrar\n\n");
      scanf("%d",&opc);
      getchar();
      
        if(opc==1){
          do{
            posi = login(cliente, nreg);
            printf("[1]ver saldo\n[2]adicionar saldo\n[3]fazer pagamento\n");
            printf("[4]sacar dinheiro\n[5]extrato bancario\n[6]sair da conta\n");
            scanf("%d",&opc);
            getchar();
            
        switch(opc){
          case 1:
            printf("Saldo na Conta: R$%.2lf\n\n", cliente[posi].saldo);
            break;
          
          case 2:
            printf ("Dinheiro a ser adicionado: ");
            scanf ("%lf", &quantidade);
            getchar();

            cliente[posi].saldo = cliente[posi].saldo + quantidade;
            printf("\nNovo saldo: R$%.2lf\n", cliente[posi].saldo);
            break;
          
          case 3:
          do{
            printf ("\nDigite o numero do usuario que recebera o pagamento: ");
            scanf ("%d", &u);
            getchar();
            
            if(u<=nreg && u!=0){
              printf ("\nVoce quer transferir para %s? (s/n)\n",cliente[u-1].nome);
              scanf("%c", &r);
              getchar();
              r = toupper(r);
                if (r == 'N')
                  break;
                else if(r != 'S'){
                  printf ("\nCaractere invalido, digite novamente!\n");
                  continue;
                }
              printf ("\nDigite a quantidade a ser transferida: ");
              scanf ("%lf", &quantidade);
              getchar();
                if(quantidade>cliente[u-1].saldo){
                  printf("Saldo isuficiente.");
                  break;
                }
              cliente[posi].saldo = cliente[posi].saldo - quantidade;
              cliente[u-1].saldo = cliente[u-1].saldo + quantidade;

              printf("\nTransacao concluida!\n");
            }
            else if(u>nreg){
              printf("O usuario nao consta em nossos registros");
              volta = 1;
            }
            else{
              printf("Numero invalido.tente novamente");
              volta = 1;  
            }
          }while(volta);            
          break;
          
          case 4:
            printf("Digite a quantidade a ser sacada: ");
            scanf ("%lf",&quantidade);
            getchar();
            if (quantidade <= cliente[posi].saldo && quantidade > 0)
            {
              cliente[posi].saldo = cliente[posi].saldo - quantidade;
              printf("\nTransacao concluida!\n");
            }
            else
              printf ("\nSaldo insuficiente!\n");
            break;
          
          case 5:
            break;

          case 6:
            sair = 0;
            break;
          
          default:
            printf("Opcao invalida.Tente novamente");
        }       
      opc = -1; //reseta a variavel opc para ser usada novamente
    }while(sair);
        }  
          else if(opc==2){
            cadastro(cliente, &nreg);
            grava_arquivo(arq, cliente, nreg);
            
          }
          else{
            printf("Opcao invalida.Tente novamente");
          }
        
  }while(programa);   
  return 0;
}
int login(tp_cliente*cliente,int n){ // n: a quantidade de clientes ja cadastrados 
  char numero[45], senha[30];
  int pos;
  int a;
  int marc=1;  //marcador comeca valendo 1 para encerrar o laco se tudo funcionar
  
      do{
        printf("Digite o numero da sua conta:\n");
        fgets(numero,40,stdin);
        a = atoi(numero);
          for(int i = 0; i<=n; i++){
            if(a == cliente[i].num){  //se encontrar o cliente no vetor
              pos = i;
              break;
            }
            else if(i==n && a != cliente[i].num){   //se o "for" tiver lido tudo e nao encontrar nada
              printf("Usuario não encontrado.Tente novamente.");
              pausa();
              marc=0;   //retornar o laco se nao encontrar nada 
            }
          }
        if(marc==1){  //se houver encontrado, pedir senha
          printf("Insira sua senha:");
          fgets(senha,30,stdin);
            if(senha == cliente[pos].senha){  //se a posicao do cliente tiver a senha correspondente
              printf("Senha correta!");
              printf("Bem vindo, %s",cliente[pos].nome);
              pausa();
            }
            else{
              printf("Senha invalida. Tente novamente.");
              pausa();
              marc=0; //retomar para numero da conta 
            }     
        }  
    } while(marc); 
    return pos;   //retorna a posicao do cliente no vetor 
}

//Criptografa os dados do usuario
void code (char* palavra)
{
  int c = 0;
  
  for (int i = 0; i < 50; i++)
  {
      if (palavra[i] != '\0')
          palavra[i]=(palavra[i]+(c%3+1));
      else
          break;
      c++;
  }
}

//Descriptografa
void decode (char* palavra) 
{
int c = 0;
  for (int i = 0; i < 30; i++)
  {
      if (palavra[i] != '\0')
          palavra[i]=(palavra[i]-(c%3+1));
      else
          break;
      c++;
  }
}

//Salva os dados do cliente no aruivo
void grava_arquivo (FILE* arqin, tp_cliente* cliente, int nreg) 
{
  arqin = fopen (CAD, "wb");

  if (arqin == NULL)
  {
    printf ("\n#ERRO NA ABERTURA DO ARQUIVO#\n");
    exit(1);
  }
  //criptografa nome e senha do usuario
  for (int i = 0; i < nreg; i++)
    {
      code (cliente[i].nome); 
      code (cliente[i].senha);
    }

    printf ("LINHA 240 NOME %s SENHA %s", cliente[1].nome, cliente[1].senha);

  //escreve o numero de usuarios cadastrados 
  fwrite(&nreg, sizeof(int), 1, arqin);
  
  //escreve os dados dos clientes no arquivo
  for (int i = 0; i < nreg; i++)
    {
      fwrite(cliente, sizeof(tp_cliente), nreg, arqin);
    }  
  printf ("\n\n LINHA 250 DEBUG: !!!! %s %s\n\n", cliente[0].nome, cliente[0].senha);//DEBUG
  fclose (arqin);
}

//Le o arquivo para passar os dados dos clientes
void le_arquivo (FILE* arqin, tp_cliente* cliente, int nreg)
{
  arqin = fopen (CAD, "rb");

  if (arqin == NULL)
  {
    printf ("\n#ERRO NA ABERTURA DO ARQUIVO#\n");
    exit(1);
  }

  printf ("\n\n\n  LINHA 266 DEBUG Numero de clientes %d \n\n", nreg); //DEBUG

  //atribui os clientes ja existentes
  for (int i = 0; i < nreg; i++)
    {
      fread(cliente, sizeof(tp_cliente), nreg, arqin); 
    }

   printf ("\n\n LINHA 273 DEBUG: !!!! %s %s\n\n", cliente[0].nome, cliente[0].senha);//DEBUG

  //Descriptografa os nomes e senhas
  for (int i = 0; i < nreg; i++)
    {
      decode (cliente[i].nome);
      decode (cliente[i].senha);
    }
  
  fclose (arqin);
}
void le_numero (FILE* arqin, int *nreg)
{
  arqin = fopen (CAD, "rb");

  if (arqin == NULL)
  {
    printf ("\n#ERRO NA ABERTURA DO ARQUIVO#\n");
    exit(1);
  }
  //Le o numero de clientes
  fread(nreg, sizeof(int), 1, arqin);
  printf ("LINHA 295 DEBUG NREG = %d", *nreg);
  fclose (arqin);
}

//Cadastrar novo usuário
void cadastro (tp_cliente* cliente, int* nreg)
  {
    //Realoca memoria para um novo bloco da struct
    (*nreg)++;
    printf ("\n\n LINHA 303 DEBUG %d\n\n", *nreg);
    cliente = (tp_cliente*) realloc (cliente, *nreg * sizeof(cliente));
    
    printf("\nDigite seu nome: ");
    
    fflush(stdin);
    fgets(cliente[*nreg-1].nome, 37, stdin);
    cliente[*nreg-1].nome[strlen(cliente[*nreg-1].nome)-1] = '\0'; //Substitui o \n por um \0
    printf (" LINHA 296 DEBUG %s", cliente[*nreg-1].nome);

    printf("\nDigite sua senha: ");
    fflush(stdin);
    fgets(cliente[*nreg-1].senha, 27, stdin);
    cliente[*nreg-1].senha[strlen(cliente[*nreg-1].senha)-1] = '\0'; //Substitui o \n por um \0
    
    printf ("LINHA 318 DEBUG %s\n", cliente[*nreg-1].nome);
    printf ("LINHA 319 DEBUG %s\n", cliente[*nreg-1].senha);

    cliente[*nreg-1].num = *nreg;
    cliente[*nreg-1].saldo = 0;
  }

//DIDATICO
void lista_cliente (tp_cliente* cliente, int nreg)
{
  for (int i = 0; i < nreg; i++)
    {
      printf("\nCliente: %s\n", cliente[i].nome);
      printf("Senha: %s\n", cliente[i].senha);
      printf("Numero: %d\n", cliente[i].num);
      printf("Saldo: %.2lf\n", cliente[i].saldo);
    }
}
