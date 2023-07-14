#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include <time.h>
#define CAD "dados.bin"

//Estrutura que armazena os dados do cliente
typedef struct{
  char senha[30];
  int num;
  char nome[40];
  double saldo;
  int ntransac;  
} tp_cliente;

//Estrutura que armazena as transacoes do cliente
typedef struct{
char data[25];  
double valor;
} t_extrato;

void code (char* );
void decode(char* );
int login(tp_cliente*, int);
void add_saldo(tp_cliente*, int, t_extrato**, struct tm*);
void sacar(tp_cliente*, int, t_extrato**, struct tm*);
void transferencia(tp_cliente*, int, int, t_extrato**, struct tm*);
void grava_arquivo (tp_cliente*, int, t_extrato**);
void le_arquivo(tp_cliente*, int, t_extrato**);
void le_numero(int*);
void cadastro(tp_cliente*, int);
void lista_cliente(tp_cliente*, int);
void extrato (tp_cliente*, t_extrato **, int, struct tm*, double);
void grava_ext (tp_cliente*, t_extrato**, int);
void verif();

//Funcao de espera que aguarda o usuario digitar algo e apos isso limpa a tela do console
void pausa()  
{ 
  char wait[5];
  
    fflush(stdin);
    fgets(wait,3,stdin);
    system("clear"); // Funcao apenas disponivel no Windows
}

int main(void)  
{
//Funcoes da time.h para indicar o horario local atual
setenv("TZ", "America/Sao_Paulo", 1);
time_t rawtime;
time( &rawtime );
struct tm *info;
info = localtime(&rawtime);

  int opc1, posi, sair=1, programa=1, nreg, opc2;      
  tp_cliente *cliente;       
  t_extrato **ext;

  //Verifica se o arquivo existe, caso nao exista, cria o arquivo 
  verif(); 
  
  //busca no cabecalho do arquivo o numero de clientes registrados
  le_numero(&nreg);  
  cliente = (tp_cliente*) calloc (nreg, sizeof(tp_cliente));  
  ext = (t_extrato**) calloc (nreg, sizeof(t_extrato*));  
  le_arquivo(cliente, nreg, ext);  //puxa os dados dos clientes do arquivo
  
    do{
      printf("Bem vindo ao banco secsafe!\n");
      printf("[1]Login\n");
      printf("[2]Cadastrar\n");
      printf("[3]Listar clientes\n");
      printf("[4]Fechar programa\n");
      scanf("%d",&opc1);
      getchar();
      system("clear");
  
        if(opc1==1){
          posi = login(cliente, nreg);  //login retorna a posicao do cliente no vetor da struct
          do{
            sair = 1;
            printf("[1]ver saldo\n[2]adicionar saldo\n[3]fazer pagamento\n");
            printf("[4]sacar dinheiro\n[5]extrato bancario\n[6]sair da conta\n");
            scanf("%d",&opc2);
            getchar();
            system("clear");
            
        switch(opc2){
          case 1:
            printf("Saldo na Conta: R$%.2lf\n\n", cliente[posi].saldo);
            pausa();   
            break;
          
          case 2:
            (cliente[posi].ntransac)++;
            ext[posi] = (t_extrato*) realloc (ext[posi], cliente[posi].ntransac * sizeof(t_extrato));
            add_saldo(cliente, posi, ext, info);
            grava_arquivo(cliente, nreg, ext);
            break;
          
          case 3:
            (cliente[posi].ntransac)++;
            ext[posi] = (t_extrato*) realloc (ext[posi], cliente[posi].ntransac * sizeof(t_extrato));
            transferencia(cliente, nreg, posi, ext, info); 
            grava_arquivo(cliente, nreg, ext);         
            break;
          
          case 4:
            (cliente[posi].ntransac)++;
            ext[posi] = (t_extrato*) realloc (ext[posi], cliente[posi].ntransac * sizeof(t_extrato));
            sacar(cliente, posi, ext, info);
            grava_arquivo(cliente, nreg, ext);
            break;
          
          case 5:
            grava_ext (cliente, ext, posi);
            break;

          case 6:
            sair = 0;
            break;
          
          default:
            printf("Opcao invalida.Tente novamente");
            pausa();
        }       
    }while(sair);
        }  
          else if(opc1==2){
            nreg++;
            cliente = (tp_cliente*) realloc (cliente, nreg * sizeof(tp_cliente));
            cadastro(cliente, nreg);
            grava_arquivo(cliente, nreg, ext);
            
          }
          else if(opc1==3)
            lista_cliente(cliente, nreg);

          else if(opc1==4) 
          {
            return 0;
          }

          else
          {
            printf("Opcao invalida.Tente novamente\n");
            pausa();
          }  
  }while(programa);   
}

int login(tp_cliente*cliente,int n)   // n:a quantidade de clientes ja cadastrados 
{ 
  char numero[45], senha[30];
  int pos;
  int a;
  int marc=0;  //marcador comeca valendo 0 para encerrar o laco se tudo funcionar
  
  do{
    printf("Digite o numero da sua conta:\n");
    fgets(numero,40,stdin);
    a = atoi(numero);
      for(int i = 0; i<=n; i++){
        if(a != 0 && a == cliente[i].num) {  //se encontrar o cliente no vetor
          pos = i;
          marc=0;
          break;
        }
        else if(i==n && a != cliente[i].num){   //se o "for" tiver lido tudo e nao encontrar nada
          printf("Usuario nao encontrado.Tente novamente.");
          pausa();
          marc=1;   //retornar o laco se nao encontrar nada 
        }
      }
    if(marc==0){  //se houver encontrado, pedir senha
      printf("Insira sua senha:");
      fgets(senha,30,stdin);
      strtok(senha,"\n");
      system("clear");
        if(!strcmp(senha,cliente[pos].senha)){  //se a posicao do cliente tiver a senha correspondente
          printf("Senha correta!\n");
          printf("Bem vindo(a), %s",cliente[pos].nome);
          pausa();
          marc=0;
        }
        else{
          printf("Senha invalida. Tente novamente.");
          pausa();
          marc=1; //retomar para numero da conta 
        }     
    }  
  }while(marc); 
  return pos;   //retorna a posicao do cliente no vetor 
}

//Adiciona dinheiro no saldo do cliente 
void add_saldo(tp_cliente* cliente, int posi, t_extrato **ext, struct tm* info){
  double quantidade;
  
    printf("Dinheiro a ser adicionado: ");
    scanf("%lf", &quantidade);
    getchar();
  
    cliente[posi].saldo = cliente[posi].saldo + quantidade;
    printf("\nNovo saldo: R$%.2lf\n", cliente[posi].saldo);
    extrato (cliente, ext, posi, info, quantidade);
    pausa();
}
/*Transfere dinheiro de um cliente para outro.
O cliente logado indica o numero da conta,após isso, ele deve confirmar se o nome está correto e selecionar a quantia a ser transferida.*/
void transferencia(tp_cliente* cliente, int nreg, int posi, t_extrato **ext, struct tm* info){
  int user;
  double quantidade;
  char resp;
  
  do{
      printf("\nDigite o numero do usuario que recebera o pagamento: ");
      scanf("%d", &user);
      getchar();
      
      if(user<=nreg && user!=0){
        printf("\nVoce quer transferir para %s? (s/n)\n",cliente[user-1].nome);
        scanf("%c", &resp);
        getchar();
        resp = toupper(resp);
        
        if (resp == 'N')
          break;
        else if(resp != 'S'){
          printf ("\nCaractere invalido, digite novamente!\n");
          pausa();
          continue;
        }
        
        printf("\nDigite a quantidade a ser transferida: ");
        scanf("%lf", &quantidade);
        getchar();
        
        if(quantidade>cliente[posi].saldo){
          printf("Saldo isuficiente.");
          pausa();
          break;
        }
        
        cliente[posi].saldo = cliente[posi].saldo - quantidade;
        cliente[user-1].saldo = cliente[user-1].saldo + quantidade;

        printf("\nTransacao concluida!\n");
        extrato (cliente, ext, posi, info, (-1)*(quantidade));
        pausa();
        break;
      }
      else if(user>nreg){
        printf("O usuario nao consta em nossos registros");
        pausa();
        break;
      }
      else{
        printf("Numero invalido.tente novamente");
        pausa(); 
        break;
      }
  }while(1);
}

//Retira o dinheiro da conta logada
void sacar(tp_cliente* cliente, int posi, t_extrato **ext, struct tm* info){
  double quantidade;
  
    printf("Digite a quantidade a ser sacada: ");
    scanf("%lf",&quantidade);
    getchar();
      if (quantidade <= cliente[posi].saldo && quantidade > 0)
      {
        cliente[posi].saldo = cliente[posi].saldo - quantidade;
        printf("\nTransacao concluida!\n");
        extrato (cliente, ext, posi, info, (-1)*(quantidade));
        pausa();
      }
      else{
        printf ("\nSaldo insuficiente!\n");
        pausa();
      }
}

//Criptografa os dados do usuario por uma cifra de cesar que acrescente o numero ASCII de cada letra da string em 1, 2 ou 3 caracteres posteriores 
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

//Descriptografa os dados fazendo o inverso da funcao 'code'
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

//Salva os dados do cliente no arquivo binário 
void grava_arquivo (tp_cliente* cliente, int nreg, t_extrato** ext) 
{
  FILE* arqin;
  arqin = fopen (CAD, "wb");

  if (arqin == NULL)
  {
    printf ("\n#ERRO NA ABERTURA DO ARQUIVO#\n");
    exit(1);
  }

  //Criptografa nome e senha do usuario para ser salvo no arquivo
  for (int i = 0; i < nreg; i++)
    {
      code (cliente[i].nome); 
      code (cliente[i].senha);
    }
  
  //Escreve o numero de usuarios cadastrados 
  fwrite(&nreg, sizeof(int), 1, arqin);

  //Escreve as estruturas que contem as informacoes de cada usuario
  for (int i = 0; i < nreg; i++)  
  {
    fwrite(&cliente[i], sizeof(tp_cliente), 1, arqin);
  }

  //Os dados precisam ser descriptografados para continuarem legiveis no programa 
  for (int i = 0; i < nreg; i++)
    {
      decode (cliente[i].nome);
      decode (cliente[i].senha);
    }

  //Escreve as estruturas com as transacoes feitas por cada usuario
   for (int i = 0; i < nreg; i++)
    {
      for (int j = 0; j < cliente[i].ntransac; j++)
        {
          fwrite (&ext[i][j], sizeof(t_extrato), 1, arqin);
        }
  }
  fclose (arqin);
}

//Le o arquivo para passar os dados dos clientes para o banco de dados no inicio do programa
void le_arquivo (tp_cliente* cliente, int nreg, t_extrato** ext)
{
  FILE* arqin;
  int b;
  arqin = fopen (CAD, "rb");

  if (arqin == NULL)
  {
    printf ("\n#ERRO NA ABERTURA DO ARQUIVO#\n");
    exit(1);
  }

  fread(&b, sizeof(int), 1, arqin);

  //Atribui as informacoes dos clientes ja exitentes
  for (int i = 0; i < nreg; i++)
   {
      fread(&cliente[i], sizeof(tp_cliente), 1, arqin); 
   }

  //Descriptografa os nomes e senhas
  for (int i = 0; i < nreg; i++)
    {
      decode (cliente[i].nome);
      decode (cliente[i].senha);
    }

  //Alocamos memoria para guardar cada transacao feita por cada usuario
  for (int i = 0; i < nreg; i++)
    {
      ext[i] = (t_extrato*)malloc(cliente[i].ntransac * sizeof(t_extrato));
      for (int j = 0; j < cliente[i].ntransac; j++)
        {
          fread(&ext[i][j], sizeof(t_extrato), 1, arqin);
        }
    }   
  
  fclose (arqin);
}

//Le a quantidade de clientes no arquivo para que o programa possa alocar memoria e guardar os dados dos usuarios
void le_numero (int *nreg)
{
  FILE* arqin;
  arqin = fopen (CAD, "rb");

  if (arqin == NULL)
  {
    printf ("\n#ERRO NA ABERTURA DO ARQUIVO#\n");
    exit(1);
  }
  
  fread(nreg, sizeof(int), 1, arqin);
  fclose (arqin);
}

//Cadastra novo usuario no sistema adquirindo seu nome e senha pela funcao fgets, como essa funcao armazena o caractere '\n' na string, utilizamos a funcao strtok para retirar o '\n' das strings, alem disso damos um numero ao cliente e inicializamos o saldo como 0
void cadastro (tp_cliente* cliente, int nreg)
  {
    printf("\nDigite seu nome: ");
    fflush(stdin);
    fgets(cliente[nreg-1].nome, 37, stdin);
    strtok(cliente[nreg-1].nome, "\n"); 

    printf("\nDigite sua senha: ");
    fflush(stdin);
    fgets(cliente[nreg-1].senha, 27, stdin);
    strtok(cliente[nreg-1].senha, "\n");
    
    cliente[nreg-1].num = nreg;
    cliente[nreg-1].saldo = 0;
    
    system("clear");
    printf("Cadastro realizado com sucesso!!\n");
    printf("O numero da sua conta e %d \n",nreg);
    pausa();
  }

//DIDATICO! Essa funcao e apenas para mostrar que os dados dos usuarios estao sendo guardados corretamente 
void lista_cliente (tp_cliente* cliente, int nreg)
{
  for (int i = 0; i < nreg; i++)
    {
      printf("\nCliente: %s\n", cliente[i].nome);
      printf("Senha: %s\n", cliente[i].senha);
      printf("Numero: %d\n", cliente[i].num);
      printf("Saldo: %.2lf\n", cliente[i].saldo);
      printf("Numero de transacoes: %d\n", cliente[i].ntransac);
    }
  pausa();
}

//Guarda na matriz de extratos, com a linha sendo o numero do cliente e a coluna o numero de transacoes feitas pelo cliente, a ultima transferencia feita pelo cliente salvando a data atual como uma string em ext.data e a quantidade de dinheiro em ext.valor
void extrato (tp_cliente* cliente, t_extrato **ext, int ncliente, struct tm *data, double valor)
{
  strcpy(ext[ncliente][cliente[ncliente].ntransac - 1].data, asctime(data));
  ext[ncliente][cliente[ncliente].ntransac - 1].valor = valor;
}

//Grava no arquivo txt que sera exibido ao cliente as transacoes feitas por ele com a data e o valor transferido
void grava_ext (tp_cliente* cliente, t_extrato** ext, int ncliente)
{
  FILE* arqout;
  arqout = fopen ("Extrato.txt", "wt");

  if (arqout == NULL)
  {
    // Caso o arquivo não exista, cria 
    arqout = fopen("Extrato.txt", "wt"); 
    fclose(arqout);
        
    // Abre o arquivo novamente para escrita
    arqout = fopen("Extrato.txt", "at");
  }
    
  for (int j = 0; j < cliente[ncliente].ntransac; j++)
  {
        fprintf (arqout, "%sValor: %.2lf", ext[ncliente][j].data, ext[ncliente][j].valor);
        fprintf (arqout, "\n");
  }
    printf("Extrato impresso com sucesso!\n");
    fclose (arqout);
}

void verif ()
{
    // Verifica se o arquivo CAD existe
    FILE *arqin = fopen(CAD, "rb");

    if (arqin == NULL)
     {
        // Caso o arquivo não exista, cria-o e define o número de registros como zero
        arqin = fopen(CAD, "wb");
        int nreg = 0;
        fwrite(&nreg, sizeof(int), 1, arqin);
        fclose(arqin);
    } 
    else
     {
        // Caso o arquivo exista, fecha-o para prosseguir com as operações normais
        fclose(arqin);
    }
}
