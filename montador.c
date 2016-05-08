/*Universidade de Brasília
Departamento de Ciência da Computação
Software Básico - Turma B - 1/2016
Professor Bruno Macchiavello
1o Trabalho Prático
GRUPO: Camila Imbuzeiro Camargo - 13/0104868
       Kilmer Luiz Aleluia      - 13/0119024 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*Facilitando a minha vida*/
#define SECTION 0
#define SPACE   1
#define CONST   2
#define PUBLIC  3
#define EXTERN  6
#define BEGIN   7
#define END     8
#define NDIR    9
#define NINSTR  14
#define MASCARA 100
#define ADD	101
#define SUB	102
#define MULT	103
#define DIV	104
#define JMP	105
#define JMPN	106
#define JMPP	107
#define JMPZ	108
#define COPY	109
#define LOAD	110
#define STORE	111
#define INPUT	112
#define OUTPUT	113
#define STOP	114
/*Protótipos*****************************************************************************************************************************/
typedef struct tabinstr tabinstr;
typedef struct tabdir tabdir;
typedef struct tabsimb tabsimb;
typedef struct tabdef tabdef;
typedef struct tabuso tabuso; 
typedef struct contadores contadores;
typedef struct objeto objeto;
typedef struct flags flags;
typedef struct panalise panalise;
typedef struct map map;
void pre_processador(FILE *fp, char* token, char* codigo_apos_pre_processamento, contadores* cont, flags* flgs);
void retira_comentario(FILE *fp, char* codigo_apos_pre_processamento, contadores* cont);
void ignorar_proxima_linha(FILE* fp, contadores* cont, char* codigo_apos_pre_processamento);
void tratamento_equ(FILE *fp, char* codigo_apos_pre_processamento, char* token, map* mapa, int indice_mapa, char* auxiliar_rotulo, contadores* cont, flags* flgs);
void tratamento_if(FILE *fp, char* token, int indice_mapa, map* mapa, char* codigo_apos_pre_processamento, contadores *cont, flags* flgs);
int verifica_substituicao_equ(int indice_mapa, char* token, map* mapa, char* codigo_apos_pre_processamento, contadores* cont);
int conferir_quebra_linha(FILE* fp, char* codigo_apos_pre_processamento, contadores* cont);
void conferir_espaco_branco(FILE* fp, char* codigo_apos_pre_processamento, contadores* cont);
void pulaEspacos(char* fonte, contadores* cont);
void transformaMaiusculo (char* token, int comprimento);
void inicializaContadores (contadores* cont);
void inicializaFlags (flags* flg);
void inicializaObjeto(objeto* obj);
char* pegaToken(char* fonte, contadores* cont);
int primeirosPassos(char* fonte, contadores* cont, tabsimb* ts, panalise* retorno, flags* flg);
int verificaRotulo(char* token, contadores* cont);
tabsimb* incluiTabelaSimbolos (tabsimb* ts, contadores* cont, char* token, int d, int a);
tabdef* incluiTabelaDefinicao (tabdef* td, contadores* cont, char* token);
tabuso* incluiTabelaUso (tabuso* tu, contadores* cont, char* token);
int procuraDiretiva(char* token, tabdir* dir);
int procuraInstrucao(char* token, tabinstr* instr);
int procuraTabelaSimbolos (tabsimb* ts, contadores* cont, char* token);
tabdef* trataDiretivaText(char* fonte, int temrot, int posdirtab, tabsimb* ts, flags* flg, tabdef* td, int auxs, contadores* cont, tabdir* dir, tabinstr* instr, int* sectionend);
int procuraSomaVetor(char* fonte, contadores* cont, flags* flg, int copy);
void trataCOPY(char* fonte, objeto* obj, contadores* cont, panalise* retorno, flags* flg, tabsimb* ts, tabuso* tu, tabinstr* instr, tabdir* dir, int posinstrtab);
void trataInstrucaoText(char* fonte, objeto* obj, contadores* cont, panalise* retorno, flags* flg, tabsimb* ts, tabuso* tu, tabinstr* instr, tabdir* dir, int posinstrtab);
panalise analisaLinhaText(char* fonte, contadores* cont, flags* flg, tabsimb* ts, tabdir* dir, tabdef* td, tabinstr* instr, objeto* obj, tabuso* tu);
void trataDiretivaData(char* fonte, objeto* obj, contadores* cont, flags* flg, tabdir* dir, tabinstr* instr, int posdirtab, int* end);
tabsimb* analisaLinhaData(char* fonte, contadores* cont, objeto* obj, flags* flg, tabdir* dir, tabinstr* instr, tabsimb* ts, int* end);
void arrumaObjeto(objeto* obj, tabsimb* ts, contadores* cont, flags* flg, tabuso* tu);
void arrumaTabDef(tabsimb* ts, tabdef* td, contadores* cont, flags* flg);
void printObjeto(objeto* obj, tabdef* td, tabuso* tu, char* arquivo, contadores* cont, flags* flg);
void monta(char* fonte, char* arquivo, int erropreproc);
tabinstr* montaTabelaInstrucoes();
tabdir* montaTabelaDiretivas(); 
/****************************************************************************************************************************************/


/*Tabela de Instruções-TESTADO(1)********************************************************************************************************/
/****************************************************************************************************************************************/

struct tabinstr {	   /*Armazenar o nome da instrução para referências futuras*/
	char instrucao[7]; /*A maior instrução tem 6 caracteres (OUTPUT). A sétima posição é para o '\0'*/
	int operandos;	   /*Número de operandos que a instrução tem*/
	int opcode;	   /*Opcode da instrução (vai de 0 a 14)*/
	int tamanho;	   /*Espaço que a instrução completa ocupa na memória. Referência para o contador de programa (PC)*/
};

typedef struct tabinstr tabinstr; /*Essa struct vai representar a tabela de instruções que será usada como referência na hora de análise dos tokens. Uma função irá definir essa tabela no início do programa*/

tabinstr* montaTabelaInstrucoes() {

	int i; /*variável auxiliar de loop*/
	tabinstr* ti;

	ti = (tabinstr*) calloc(14,(sizeof(tabinstr))); /*Alocando os 14 espaços da tabela, já que temos 14 instruções*/

	for(i=0;i<14;i++) {		/*montaTabelaInstrucoes - LOOP 1*/
		ti[i].operandos = 1;	/*Não vale para COPY e STOP mas vale para todas as outras instruções. Arruma-se depois!*/
		ti[i].tamanho = 2;	/*Não vale para COPY e STOP, mas isso será arrumado depois. Assim fica mais fácil p/ as outras.*/
		ti[i].opcode = i+1;	/*colocando em ordem todos os opcodes utilizando a própria variável do loop pois as variáveis vão de 1 a 14. Ex: elemento 0 do vetor é ADD e tem opcode 1 = 0 + 1*/
	}

	/*Escrevendo a string para ADD, opcode = 1*/
	ti[0].instrucao[0]='A';
	ti[0].instrucao[1]='D';
	ti[0].instrucao[2]='D';
	ti[0].instrucao[3]='\0';
	/*Escrevendo a string para SUB, opcode = 2*/
	ti[1].instrucao[0]='S';
	ti[1].instrucao[1]='U';
	ti[1].instrucao[2]='B';
	ti[1].instrucao[3]='\0';
	/*Escrevendo a string para MULT, opcode = 3*/
	ti[2].instrucao[0]='M';
	ti[2].instrucao[1]='U';
	ti[2].instrucao[2]='L';
	ti[2].instrucao[3]='T';
	ti[2].instrucao[4]='\0';
	/*Escrevendo a string para DIV, opcode = 4*/
	ti[3].instrucao[0]='D';
	ti[3].instrucao[1]='I';
	ti[3].instrucao[2]='V';
	ti[3].instrucao[3]='\0';
	/*Escrevendo a string para JMP, opcode = 5*/
	ti[4].instrucao[0]='J';
	ti[4].instrucao[1]='M';
	ti[4].instrucao[2]='P';
	ti[4].instrucao[3]='\0';
	/*Escrevendo a string para JMPN, opcode = 6*/
	ti[5].instrucao[0]='J';
	ti[5].instrucao[1]='M';
	ti[5].instrucao[2]='P';
	ti[5].instrucao[3]='N';
	ti[5].instrucao[4]='\0';
	/*Escrevendo a string para JMPP, opcode = 7*/
	ti[6].instrucao[0]='J';
	ti[6].instrucao[1]='M';
	ti[6].instrucao[2]='P';
	ti[6].instrucao[3]='P';
	ti[6].instrucao[4]='\0';
	/*Escrevendo a string para JMPZ, opcode = 8*/
	ti[7].instrucao[0]='J';
	ti[7].instrucao[1]='M';
	ti[7].instrucao[2]='P';
	ti[7].instrucao[3]='Z';
	ti[7].instrucao[4]='\0';
	/*Escrevendo a string para COPY, opcode = 9 e corrigindo o tamanho e o número de operandos*/
	ti[8].instrucao[0]='C';
	ti[8].instrucao[1]='O';
	ti[8].instrucao[2]='P';
	ti[8].instrucao[3]='Y';
	ti[8].instrucao[4]='\0';
	ti[8].tamanho = 3;
	ti[8].operandos = 2;
	/*Escrevendo a string para LOAD, opcode = 10*/
	ti[9].instrucao[0]='L';
	ti[9].instrucao[1]='O';
	ti[9].instrucao[2]='A';
	ti[9].instrucao[3]='D';
	ti[9].instrucao[4]='\0';
	/*Escrevendo a string para STORE, opcode = 11*/
	ti[10].instrucao[0]='S';
	ti[10].instrucao[1]='T';
	ti[10].instrucao[2]='O';
	ti[10].instrucao[3]='R';
	ti[10].instrucao[4]='E';
	ti[10].instrucao[5]='\0';
	/*Escrevendo a string para INPUT, opcode = 12*/
	ti[11].instrucao[0]='I';
	ti[11].instrucao[1]='N';
	ti[11].instrucao[2]='P';
	ti[11].instrucao[3]='U';
	ti[11].instrucao[4]='T';
	ti[11].instrucao[5]='\0';
	/*Escrevendo a string para OUTPUT, opcode = 13*/
	ti[12].instrucao[0]='O';
	ti[12].instrucao[1]='U';
	ti[12].instrucao[2]='T';
	ti[12].instrucao[3]='P';
	ti[12].instrucao[4]='U';
	ti[12].instrucao[5]='T';
	ti[12].instrucao[6]='\0';
	/*Escrevendo a string para STOP, opcode = 14 e corringindo o tamanho e o número de operandos*/
	ti[13].instrucao[0]='S';
	ti[13].instrucao[1]='T';
	ti[13].instrucao[2]='O';
	ti[13].instrucao[3]='P';
	ti[13].instrucao[4]='\0';
	ti[13].tamanho = 1;
	ti[13].operandos = 0;

	return ti;
}

/*Tabela de Diretivas-TESTADO(1)*********************************************************************************************************/
/****************************************************************************************************************************************/

struct tabdir {		  /*Armazena as diretivas para referências futuras na hora de traduzir*/
	char diretiva[8]; /*A maior referência é SECTION que tem 7 letras. O oitavo espaço no vetor existe para armazenar o '\0'*/
	int operandos;	  /*Número de "operandos" da diretiva*/
	int tamanho;	  /*Espaço em memória ocupado pela diretiva*/
};

typedef struct tabdir tabdir;

tabdir* montaTabelaDiretivas() {

	tabdir* td;

	td = (tabdir*) calloc(9,(sizeof(tabdir)));

	/*Incluindo os dados da diretiva SECTION*/
	td[0].diretiva[0]='S';
	td[0].diretiva[1]='E';
	td[0].diretiva[2]='C';
	td[0].diretiva[3]='T';
	td[0].diretiva[4]='I';
	td[0].diretiva[5]='O';
	td[0].diretiva[6]='N';
	td[0].diretiva[7]='\0';
	td[0].operandos = 1;
	td[0].tamanho = 0;
	/*Incluindo os dados da diretiva SPACE*/
	td[1].diretiva[0]='S';
	td[1].diretiva[1]='P';
	td[1].diretiva[2]='A';
	td[1].diretiva[3]='C';
	td[1].diretiva[4]='E';
	td[1].diretiva[5]='\0';
	td[1].operandos = 1;
	td[1].tamanho = 1;
	/*Incluindo os dados da diretiva CONST*/
	td[2].diretiva[0]='C';
	td[2].diretiva[1]='O';
	td[2].diretiva[2]='N';
	td[2].diretiva[3]='S';
	td[2].diretiva[4]='T';
	td[2].diretiva[5]='\0';
	td[2].operandos = 1;
	td[2].tamanho = 1;
	/*Incluindo os dados da diretiva PUBLIC*/
	td[3].diretiva[0]='P';
	td[3].diretiva[1]='U';
	td[3].diretiva[2]='B';
	td[3].diretiva[3]='L';
	td[3].diretiva[4]='I';
	td[3].diretiva[5]='C';
	td[3].diretiva[6]='\0';
	td[3].operandos = 0;
	td[3].tamanho = 0;
	/*Incluindo os dados da diretiva EQU*/
	td[4].diretiva[0]='E';
	td[4].diretiva[1]='Q';
	td[4].diretiva[2]='U';
	td[4].diretiva[3]='\0';
	td[4].operandos = 1;
	td[4].tamanho = 0;
	/*Incluindo os dados da diretiva IF*/
	td[5].diretiva[0]='I';
	td[5].diretiva[1]='F';
	td[5].diretiva[2]='\0';
	td[5].operandos = 1;
	td[5].tamanho = 0;
	/*Incluindo os dados da diretiva EXTERN*/
	td[6].diretiva[0]='E';
	td[6].diretiva[1]='X';
	td[6].diretiva[2]='T';
	td[6].diretiva[3]='E';
	td[6].diretiva[4]='R';
	td[6].diretiva[5]='N';
	td[6].diretiva[6]='\0';
	td[6].operandos = 0;
	td[6].tamanho = 0;
	/*Incluindo os dados da diretiva BEGIN*/
	td[7].diretiva[0]='B';
	td[7].diretiva[1]='E';
	td[7].diretiva[2]='G';
	td[7].diretiva[3]='I';
	td[7].diretiva[4]='N';
	td[7].diretiva[5]='\0';
	td[7].operandos = 0;
	td[7].tamanho = 0;
	/*Incluindo os dados da diretiva END*/
	td[8].diretiva[0]='E';
	td[8].diretiva[1]='N';
	td[8].diretiva[2]='D';
	td[8].diretiva[3]='\0';
	td[8].operandos = 0;
	td[8].tamanho = 0;

	return td;
}

/*Structs de Tabelas*********************************************************************************************************************/
/****************************************************************************************************************************************/

struct tabsimb {
	char simbolo[100]; /*símbolo do(a) rótulo/variável (pode ter no máximo 100 caracteres) AINDA EXISTE A POSSIBILIDADE DE EU MUDAR ISSO PARA UM PONTEIRO E FAZER O MALLOC NA HORA!!!! USAR VARIÁVEL AUXILIAR NESSE CASOOOOOO*/
	int endereco; /*endereço do(a) variável*/
	int definido; /*Booleano para marcar se o(a) rótulo/variável já foi definido(a)*/
	int externo;  /*Marcar se o rótulo/variável é externo(a) para o caso dos módulos*/
	int lastaddr; /*Eu vou incluir a lista de endereços onde aparece o símbolo no próprio código traduzido, por isso preciso sempre marcar o último endereço incluído na lista*/
};

typedef struct tabsimb tabsimb;

struct tabdef {		   /*Tabela de Definições que será criada e preenchida caso o programa seja um módulo*/
	char simbolo[100]; /*Label do token*/
	int endereco;	   /*Endereço dele no código. Essa tabela só será gerada caso haja um BEGIN-END*/
};			   /*e usará a tabela de símbolos como referência, diretiva PUBLIC sendo a que irá definir isso*/

typedef struct tabdef tabdef;

struct tabuso {
	char simbolo[100];	/*Símbolo da variável ou do rótulo externo(a)*/
	int endcod;		/*Coluna de endereços que irá definir os endereços em que eles aparecem no código*/
};				/*A diretiva EXTERN irá definir quais são os labels que eu tenho que incluir aqui*/

typedef struct tabuso tabuso;   /*Quando eu percorrer o código e encontrar um símbolo, se ele for 'externo' a tabuso*/
				/*é atualizada*/

struct contadores {		/*Struct que junta todos os contadores de linha que eu vou usar*/
	int pontodeleitura;	/*Ele foi criado meramente pra eu não ter que ficar de olho em mil variáveis diferentes*/
	int contadorlinha;	/*Facilita encontrar assim*/
	int contadorinstr;	/*INICIALIZAR TODOS EM 0*/
	int tamts;		/*tamanho da tabela de símbolos*/
	int tamtd;		/*tamanho da tabela de definições*/
	int tamtu;
	int eof;
};

typedef struct contadores contadores;

struct objeto {		/*Vetor que irá representar o código objeto*/
	int codigo;	/*Quando criado, provavelmente terá 216 posições (definido como o máximo de endereços de memória disponível)*/
	int somar;	/*Para o caso de diretivas X+2, preciso saber o quanto preciso somar*/
	int relativo;	/*Define se o endereço é relativo ou não*/
	int linha;
};

typedef struct objeto objeto;

struct flags {
	int erro;
	int modulo;
	int temstop;
	int begin;
	int end;
	int fimtext;
};

typedef struct flags flags;

struct panalise {			/*Struct para lidar com retornos de novos ponteiros caso eu precise atualizar mais*/
	tabsimb* ts;			/*de uma tabela em analisaLinhaText*/
	tabdef* td;
	tabuso* tu;
	int sectionendT;		/*indicador de quando precisamos parar de ler linhas de TEXT (p/ o LOOP de chamada)*/
	int sectionendD;		
};					/*de analisaLinhaText*/

typedef struct panalise panalise;

struct map{				/*Estrutura de mapeamento: para cada chave temos um valor*/
    char* key;
    char* value;
};

typedef struct map map;
/****************************************************************************************************************************************/
/****************************************************************************************************************************************/

/*Funções Auxiliares*********************************************************************************************************************/
/****************************************************************************************************************************************/

void pulaEspacos(char* fonte, contadores* cont) {	/*Função criada para ignorar espaços*/

	int i=(*cont).pontodeleitura;			/*Pegamos o ponto de leitura*/
							/*e enquanto houver qualquer caractere que represente um espaço em branco*/
	while (fonte[i]=='\n' || fonte[i]=='\t' || fonte[i]==' ' || fonte[i]=='\r') { /*nós o ignoramos*/
		if(fonte[i]=='\n'){
			(*cont).contadorlinha++;	/*O caractere só não é completamente ignorado se ele for um que indica nova linha*/
		}					/*Nesse caso é necessário atualizar o contador de linhas*/
		i++;					/*Continuamos percorrendo o texto até não haverem mais espaços em branco*/
	}
	(*cont).pontodeleitura=i;			/*Quando isso acontecer atualizamos a posição em que estamos realizando a leitura*/
}



void transformaMaiusculo (char* token, int comprimento) { /*TESTADO*/
	int i; /*variável auxiliar do loop*/

	for(i=0;i<comprimento;i++) {	            /*para cada elemento da string vemos se ele está em letra minúscula*/
		if(token[i]>=97 && token[i]<=122) { /*ou seja, possui o ascii entre 97 = 'a' e 122 = 'z'*/
			token[i] = token[i] - 32;   /*Se tivermos um desses casos, subtraímos 32 para obter a versão maiúscula da letra*/
		}				    /*Os outros ascii não precisam ser incluídos pois a questão maiúscula-minúscula*/
	}					    /*só envolve LETRAS*/
}


void inicializaContadores (contadores* cont) {	     /*Função auxiliar para zerar os contadores (no início são todos zero)*/
	
	(*cont).pontodeleitura = 0;
	(*cont).contadorlinha = 0;
	(*cont).contadorinstr = 0;
	(*cont).tamts = 0;
	(*cont).tamtd = 0;
	(*cont).tamtu = 0;
	(*cont).eof = 0;
}


void inicializaFlags (flags* flg) {

	(*flg).erro = 0;
	(*flg).modulo = 0;
	(*flg).temstop = 0;
	(*flg).begin = 0;
	(*flg).end = 0;
	(*flg).fimtext = 0;
}

void inicializaObjeto(objeto* obj) {

	int i;
	
	for(i=0;i<215;i++) {
		obj[i].codigo = 0;
		obj[i].somar = 0;
		obj[i].relativo = 0;
		obj[i].linha = 0;
	}
}
/****************************************************************************************************************************************/
/****************************************************************************************************************************************/



/*Funções Montador (1a (e única) passagem))**********************************************************************************************/
/****************************************************************************************************************************************/


/*Pré-processamento**********************************************************************************************************************/

/* Funcao que efetua o pre-processamento. 
*
* Parametros:
*   FILE *fp - ponteiro de arquivo que possui o codigo assembly. 
*   char *token - vetor que ira possuir o token lido do arquivo. 
*   char codigo_apos_pre_processamento - string que, ao decorrer da funcao, ira construir o codigo pre processado.
*   contadores *cont - ponteiro de contadores, para contagem das linhas e indicacao de EOF (final do arquivo)
*   flags* flgs - ponteiro de flags que contem numero de erros. 
*
* Existem comentario com "DEBUG". O primeiro deles imprime o token e informa em qual linha ele está. 
* O segundo imprime o mapa de EQU com o rotulo (key) e o valor referente a ele(value).
*
*/
void pre_processador(FILE *fp, char* token, char* codigo_apos_pre_processamento, contadores* cont, flags* flgs){

    char auxiliar_rotulo[255];
    map mapa[63];
    int indice_mapa = 0;

    while(fscanf(fp,"%s",token) != EOF){

        /* *** DEBUG *** *** DEBUG *** *** DEBUG ***
        *
        * printf("TOKEN: %s --- LINHA: %d\n", token, (*cont).contadorlinha);
        */
        transformaMaiusculo(token, strlen(token));

        if (strstr(token, ";") != NULL){
            retira_comentario(fp, codigo_apos_pre_processamento, cont);
            conferir_quebra_linha(fp, codigo_apos_pre_processamento, cont);
            continue;
        }

        if(strcmp(token, "EQU") == 0){
            int size = strlen(codigo_apos_pre_processamento);
            codigo_apos_pre_processamento[size - strlen(auxiliar_rotulo) - 1] = '\0';

            tratamento_equ(fp, codigo_apos_pre_processamento, token, mapa, indice_mapa, auxiliar_rotulo, cont, flgs);
            indice_mapa++;

            conferir_espaco_branco(fp, codigo_apos_pre_processamento, cont);
            conferir_quebra_linha(fp, codigo_apos_pre_processamento, cont);
            continue;
        }

        if(strcmp(token, "IF") == 0){
            tratamento_if(fp, token, indice_mapa, mapa, codigo_apos_pre_processamento, cont, flgs);

            conferir_espaco_branco(fp, codigo_apos_pre_processamento, cont);
            conferir_quebra_linha(fp, codigo_apos_pre_processamento, cont); 
            
            continue;
        }

        if(verifica_substituicao_equ(indice_mapa, token, mapa, codigo_apos_pre_processamento, cont)){
            conferir_espaco_branco(fp, codigo_apos_pre_processamento, cont);
            conferir_quebra_linha(fp, codigo_apos_pre_processamento, cont);          
            continue;
        }

        strcat(codigo_apos_pre_processamento, token);
        strcpy(auxiliar_rotulo, token);

        conferir_espaco_branco(fp, codigo_apos_pre_processamento, cont);
        conferir_quebra_linha(fp, codigo_apos_pre_processamento, cont);

        if((*cont).eof == 1)
            break;
    }

    /*** DEBUG *** *** DEBUG *** *** DEBUG ***
    int i=0;
    for(;i<indice_mapa;i++){
        printf("\nmapa[%d].key: %s\nmapa[%d].value: %s\n", i, mapa[i].key, i, mapa[i].value);
    }*/
}


/* Funcao que identifica o caracter de quebrar linha ('\n') e, caso exista, insere na string codigo_apos_pre_processamento
* e incrementa o contador de linha. Essencial em funcoes como 'tratamento_equ' e 'tratamento_if'.
*
* Parametros:
*   FILE *fp - ponteiro do arquivo que contem o codigo assembly original.
*   char codigo_apos_pre_processamento - string que, ao decorrer da funcao, ira construir o codigo pre processado.
*   contadores *cont - ponteiro de contadores, para contagem das linhas e indicacao de EOF (final do arquivo)
*
* Retorno: indica se houve ou nao a quebra de linha: 0, caso nao haja '\n', ou 1, caso exista(m).
*
*/
int conferir_quebra_linha(FILE* fp, char* codigo_apos_pre_processamento, contadores* cont){

    int caracter;
    int linha_quebrada = 0;
    int size = strlen(codigo_apos_pre_processamento);

    while( ((caracter = fgetc(fp)) != EOF) &&  (caracter == '\n' || caracter == '\r')) {
        if(caracter == '\n') {
		codigo_apos_pre_processamento[size] = '\n';
		codigo_apos_pre_processamento[size + 1] = '\0';
		(*cont).contadorlinha++;
		linha_quebrada = 1;

		/* Se houve quebra de linha, o tamanho da string codigo_apos_pre_processamento eh atualizado*/
		size = strlen(codigo_apos_pre_processamento);
	}
    }

    if(caracter == EOF){
        (*cont).eof = 1;
    }
    
    /* considerando que sempre um caracter a mais eh lido, o ponteiro retorna uma posicao para que os tokens nao 
    "quebrem". */
    fseek(fp, -(sizeof(char)), SEEK_CUR);
    return linha_quebrada;
}

/* Funcao que identifica caracteres em branco (' ') e, caso existam, insere-os na string codigo_apos_pre_processamento.
*
* Parametros:
*   FILE *fp - ponteiro do arquivo que contem o codigo assembly original.
*   char codigo_apos_pre_processamento - string que, ao decorrer da funcao, ira construir o codigo pre processado.
*   contadores *cont - ponteiro de contadores, para contagem das linhas e indicacao de EOF (final do arquivo)
*
*/
void conferir_espaco_branco(FILE* fp, char* codigo_apos_pre_processamento, contadores* cont){

    int caracter;
    int size = strlen(codigo_apos_pre_processamento);

    while( ((caracter = fgetc(fp)) != EOF) &&  ((caracter == ' ') || caracter == '\t') ) {
        codigo_apos_pre_processamento[size] = ' ';
        codigo_apos_pre_processamento[size + 1] = '\0';
    }

    if(caracter == EOF){
        (*cont).eof = 1;
    }

    /* considerando que sempre um caracter a mais eh lido, o ponteiro retorna uma posicao para que os tokens nao 
    "quebrem". */
    fseek(fp, -(sizeof(char)), SEEK_CUR);

}


/* Funcao que retira os comentarios do programa. 
*
* Parametros:
*   FILE *fp - ponteiro do arquivo que contem o codigo assembly original.
*   char codigo_apos_pre_processamento - string que, ao decorrer da funcao, ira construir o codigo pre processado.
*   contadores *cont - ponteiro de contadores, para contagem das linhas e indicacao de EOF (final do arquivo)
*
*/
void retira_comentario(FILE *fp, char* codigo_apos_pre_processamento, contadores* cont){
    int size = strlen(codigo_apos_pre_processamento);
    int caracter;

    /* ponteiro "anda" no arquivo enquanto nao ocorrer a quebra de linha */
    while ((caracter = fgetc(fp)) != '\n');
                
    codigo_apos_pre_processamento[size] = '\n';
    (*cont).contadorlinha++;
    codigo_apos_pre_processamento[size + 1] = '\0';
}

/* Funcao que ignora linhas do if e a linha seguinte. Essa funcao eh chamada quando um IF nao 
* sucede, ou seja, nao o rotulo adjunto ao IF nao esta na lista de rotulos EQU. 
*
* Parametros:
*   FILE *fp - ponteiro do arquivo que contem o codigo assembly original.
*   char codigo_apos_pre_processamento - string que, ao decorrer da funcao, ira construir o codigo pre processado.
*   contadores *cont - ponteiro de contadores, para contagem das linhas e indicacao de EOF (final do arquivo)
*
*/
void ignorar_proxima_linha(FILE* fp, contadores* cont, char* codigo_apos_pre_processamento){
    int caracter;
    int size = strlen(codigo_apos_pre_processamento);

    /*haverão dois \n. Um deles é após o IF, o outro é o que deve ser ignorado*/
    while((caracter = fgetc(fp)) != '\n');
//    while((caracter = fgetc(fp)) != '\n');  
    
    codigo_apos_pre_processamento[size] = '\n';
    codigo_apos_pre_processamento[size + 1] = '\n';
    codigo_apos_pre_processamento[size + 2] = '\0';
    (*cont).contadorlinha++;
    (*cont).contadorlinha++;
}


/* Funcao que adiciona o EQU no mapa de rotulo/valor. 
*
* Parametros
*   FILE *fp - ponteiro do arquivo que contem o codigo assembly original.
*   char codigo_apos_pre_processamento - string que, ao decorrer da funcao, ira construir o codigo pre processado.
*   char* token - token a ser lido para ser adicionado ao valor no mapa de rotulo/valor de EQU.
*   map* mapa - mapa que possui todos os rotulos e seus respectivos valores do programa
*   int indice_mapa - indice no mapa de rotulos(keys) e valores de EQU
*   char* auxiliar_rotulo - nome do rotulo relacionado ao EQU. 
*   contadores *cont - ponteiro de contadores, para contagem das linhas e indicacao de EOF (final do arquivo)
*   flags* flgs - ponteiro de flags para a possibilidade de erro. 
*
*/
void tratamento_equ(FILE *fp, char* codigo_apos_pre_processamento, char* token, map* mapa, int indice_mapa, char* auxiliar_rotulo, contadores* cont, flags* flgs){

    int ignorador;

    mapa[indice_mapa].key = (char*) malloc(sizeof(auxiliar_rotulo-1));

    /*Retira ';' da string*/
    auxiliar_rotulo[strlen(auxiliar_rotulo)-1] = '\0';
    strcpy(mapa[indice_mapa].key,auxiliar_rotulo);

    /* Token lido representa o valor de substituicao do rotulo do EQU*/
    fscanf(fp,"%s",token);
    transformaMaiusculo(token, strlen(token));

    /* Adiciona o valor ao mapa*/
    mapa[indice_mapa].value = (char*) malloc(sizeof(token));
    strcpy(mapa[indice_mapa].value,token);

    /*Confere existencia de diversos operandos no EQU*/
    conferir_espaco_branco(fp, codigo_apos_pre_processamento, cont);
    if(conferir_quebra_linha(fp, codigo_apos_pre_processamento, cont) == 0){
        printf("Erro sintático, linha %d. Operador EQU com número inválido de operandos. Apenas o primeiro operando será considerado.\n", (*cont).contadorlinha);
        while( ((ignorador = fgetc(fp)) != EOF) &&  ignorador != '\n');

        /* No if abaixo, significa que o arquivo terminou*/
        if(ignorador == -1){
            (*cont).eof = 1;
        } 
        /* No else abaixo, significa que o char eh um '\n'.*/
        else{
            (*cont).contadorlinha++;
        }

        (*flgs).erro++;
    }
}

/* Funcao que verifica se o IF esta certo (o rotulo EQU relacionado existe) ou errado (nao existe).
*
* Parametros
*   FILE *fp - ponteiro do arquivo que contem o codigo assembly original.
*   char* token - token a ser lido para ser adicionado ao valor no mapa de rotulo/valor de EQU.
*   int indice_mapa - indice no mapa de rotulos(keys) e valores de EQU
*   map* mapa - mapa que possui todos os rotulos e seus respectivos valores do programa
*   char codigo_apos_pre_processamento - string que, ao decorrer da funcao, ira construir o codigo pre processado.
*   contadores *cont - ponteiro de contadores, para contagem das linhas e indicacao de EOF (final do arquivo)
*   flags* flgs - ponteiro de flags para a possibilidade de erro. 
*
*/
void tratamento_if(FILE *fp, char* token, int indice_mapa, map* mapa, char* codigo_apos_pre_processamento, contadores* cont, flags* flgs){
    /*O token agora possui a chave a ser procurado no mapa*/
    fscanf(fp,"%s",token);
    transformaMaiusculo(token, strlen(token));
    
    int ignorador, j=0, encontrado = 0;

    /* procura a chave no mapa*/
    for(;j<indice_mapa;j++){
        /* No caso abaixo, a diretiva IF estava verdadeira */
        if(strcmp(token, mapa[j].key) == 0){
            encontrado = 1;
            break;
        }
    }

    /* verifica existencia de mais de um operando para a diretiva - o que significa erro sintatico*/
    conferir_espaco_branco(fp, codigo_apos_pre_processamento, cont);
    if(conferir_quebra_linha(fp, codigo_apos_pre_processamento, cont) == 0){
        encontrado = 0;
        printf("Erro sintático, linha %d: operador IF com número inválido de operandos. Apenas o primeiro operando será considerado.\n", (*cont).contadorlinha);
        while( ((ignorador = fgetc(fp)) != EOF) &&  ignorador != '\n');

        /* No if abaixo, significa que o arquivo terminou*/
        if(ignorador == -1){
            (*cont).eof = 1;
        } 
        /* No else abaixo, significa que o char eh um '\n'.*/
        else{
            (*cont).contadorlinha++;
        }

        (*flgs).erro++;
    }

    /* Nesse caso a diretiva IF estava falsa*/
    if(!encontrado || (encontrado && atoi(mapa[j].value) < 1)){
	if (!encontrado) {
		printf("Erro semântico, linha %d: operador IF possui operando não definido.\n ", (*cont).contadorlinha);
        	(*flgs).erro++;
	}
        ignorar_proxima_linha(fp, cont, codigo_apos_pre_processamento);
    }

}


/* Funcao que verifica se deve haver substituicao em decorrencia dos rotulos EQU. 
*
* Parametros
*   int indice_mapa - indice no mapa de rotulos(keys) e valores de EQU
*   char *token - token a ser procurado na lista de keys do mapa
*   map* mapa - mapa que possui todos os rotulos e seus respectivos valores do programa
*   contadores *cont - ponteiro de contadores, para contagem das linhas e indicacao de EOF (final do arquivo)
*
* Retorno - verifica se houve (retorno 1) ou nao (retorno 0) substituicao de token por um value do mapa. 
*
*/
int verifica_substituicao_equ(int indice_mapa, char* token, map* mapa, char* codigo_apos_pre_processamento, contadores* cont){
    int j=0;
    int encontrado = 0;
    for(;j<indice_mapa;j++){
        if(strcmp(token, mapa[j].key) == 0){
            encontrado = 1;
            strcat(codigo_apos_pre_processamento, mapa[j].value);
            break;
        }
    }
    return encontrado;
}




/*Montagem*******************************************************************************************************************************/

/*****************************************************************************************************************************************
Essa função é uma função que pega o próximo token do arquivo fonte, considerando-se que tokens são separados entre si por caracteres de espaçamento. A função então percorre o fonte e coleta os caracteres até que haja um espaço ou uma indicação do final do vetor fonte (para não extrapolar o limite do vetor. Afinal, depois do final do fonte não há mais o que ser lido).
Argumentos: fonte (é no fonte que temos o token), cont (a partir do ponto de leitura é que devemos começar a obtenção do token)
Retorno: uma string contendo o token obtido  
*****************************************************************************************************************************************/
char* pegaToken(char* fonte, contadores* cont) {

	int i = (*cont).pontodeleitura, j=(*cont).pontodeleitura;
	char* token;
	while(fonte[i] != '\n' && fonte[i] != '\t' && fonte[i] != ' ' && fonte[i] != '\r' && fonte[i] != '\0' ) { i++; }
	i = i - j;
	token = (char*) calloc((i+1),sizeof(char)); 
	i = 0;
	while(fonte[j] != '\n' && fonte[j] != '\t' && fonte[j] != ' ' && fonte[j] != '\r' && fonte[j] != '\0') {
		token[i] = fonte[j];
		i++;
		j++;
	}
	token[i] = '\0';
	return token;
}

/*****************************************************************************************************************************************
Essa função analisa a possibilidade de termos uma definição de módulo antes da seção de texto e ela também encontra SECTION TEXT. Então começamos pegando o primeiro token e vendo, primeiramente, se ele é um rótulo. Se ele for um rótulo ele é incluído na tabela de símbolos, e automaticamente sabemos que devemos procurar BEGIN. Se seguido desse rótulo não temos BEGIN, temos um rótulo fora da seção de textos ou de dados, o que está errado. Se o que segue esse rótulo é BEGIN, podemos indicar que temos um módulo, podemos indicar  isso para as flags. Independente de termos um módulo ou não, o próximo passo é procurar SECTION TEXT. Por isso pegamos o token e comparamos. Se não for SECTION, a diretiva está faltando e temos um erro. Não avançamos o ponto de leitura pois iremos continuar avaliando o código independente dos erros. Verificamos então o que segue SECTION. Se não for TEXT então, novamente temos um erro e o símbolo é ignorado para ser analisado em outro momento.
Chamada por: monta
Chama: pegaToken, pulaEspacos, transformaMaiusculo (para analisar tokens), incluiTabelaSimbolos (para o caso de haver a definição de um módulo)
Argumentos: fonte (o objeto de análise é o arquivo fonte), cont (os contadores são usado como auxílio), flg (para atualizar no caso de ser um módulo), ts (caso seja necessária a inclusão), retorno (caso a tabela de símbolos seja atualizada, precisamos atualizar o seu ponteiro pois haverá a realocação)
Retorno: retorna 1 se SECTION foi encontrada e 0 se não foi ou se foi encontrada com erro
*****************************************************************************************************************************************/
int primeirosPassos(char* fonte, contadores* cont, tabsimb* ts, panalise* retorno, flags* flg) { /*A primeira coisa que precisamos fazer é encontrar a seção SECTION TEXT*/

	char* token;
	int r;

	pulaEspacos(fonte,cont); /*Pulamos os espaços iniciais no texto*/
	token = pegaToken (fonte,cont); /*Pegamos o token que aparece depois desse primeiro espaço*/
	transformaMaiusculo(token,strlen(token));
	r = verificaRotulo(token, cont);
	if(r>=0) { /*caso Módulo*/
		(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);		
		token[strlen(token)-1] = '\0';
		(*retorno).ts = incluiTabelaSimbolos(ts, cont, token, 1, (-1)); 
		ts = (*retorno).ts;
		pulaEspacos(fonte,cont); /*Pulamos os espaços iniciais no texto*/
		token = pegaToken (fonte,cont); /*Pegamos o token que aparece depois desse primeiro espaço*/
		transformaMaiusculo(token,strlen(token));
			if(strcmp(token, "BEGIN")) { /*Se o que segue o rótulo não é BEGIN*/
				printf("Erro sintático, linha %d: rótulo fora da seção de texto não indica início de módulo\n", (*cont).contadorlinha);
				((*flg).erro)++;
					if(strcmp(token, "SECTION")) {
						(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);
					}
			}
			else {
				(*flg).modulo = 1;
				(*flg).begin = 1;
				(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);
			}
	}
	
	pulaEspacos(fonte,cont); /*Visto que é módulo temos que encontrar SECTION TEXT*/
	token = pegaToken (fonte,cont);
	transformaMaiusculo(token,strlen(token));

	if (strcmp(token,"SECTION") != 0) {
		printf("Erro sintático, linha %d: seção SECTION TEXT ausente ou com erro.\n", (*cont).contadorlinha);
		return 1;
	}
	else {
		(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);
//		free(token);
		pulaEspacos(fonte,cont);
		token = pegaToken(fonte,cont);
		transformaMaiusculo(token,strlen(token));
			if(strcmp(token,"TEXT") != 0) {
				printf("Erro sintático, linha %d: seção SECTION %s inválida.\n", (*cont).contadorlinha, token);
				//(*cont).pontodeleitura = (*cont).pontodeleitura - strlen(token);
				return 1;
			}
			else {
				(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);
//				free(token);
				return 0;
			}
	}


}

/*****************************************************************************************************************************************
Essa função verifica se um token é um símbolo válido. Primeiro verifica-se se o último símbolo é ':' pois rótulos se encerram com esse símbolo (para o caso de símbolos que não são rótulos, apenas enviamos uma concatenação do símbolo com ':' e avaliação será a mesma). Verificamos então se a string tem o tamanho permitido (não sendo maior do que 100). Se ela não tiver o tamanho permitido, indicamos um erro. Verificamos também se o token começa com caracteres válidos e se ele não apresenta caracteres inválidos para as outras posições.
Argumentos: token (o símbolo que será avaliado), cont (para indicar a linha do erro)
Retorno: retorna 1 se é um rótulo (ou símbolo) mas ele é inválido, retorna 0 se é um rótulo (ou símbolo que não apresenta erros), retorna um valor negativo se não é um rótulo (ou símbolo) 
*****************************************************************************************************************************************/
int verificaRotulo(char* token, contadores* cont) { /*Retorna -1 se não é rótulo, retorna 0 se é, retorna 1 se é e existe algum erro*/

	int i = strlen(token) - 1, j=0;
	
	if (token[i]==':') {
		if (strlen(token)>100) {
			printf("Erro léxico, linha %d, rótulo %s excede o número máximo de caracteres.\n",(*cont).contadorlinha, token);
			j=1;
		}
		if(token[0]<65 || token[0]>122 || token[0]==91 || token[0]==92 || token[0]==93 || token[0]==94 || token[0]==96) {
			printf("Erro léxico, linha %d: rótulo %s começa com caractere inválido.\n",(*cont).contadorlinha, token);
			j=1;
		}		
		for(i=1;i<strlen(token)-1;i++) {
			if(token[i]<48 || token[i]>122 || token[i]==91 || token[i]==92 || token[i]==93 || token[i]==94 || token[i]==96 || 				   token[i]==58 || token[i]==59 || token[i]==60 || token[i]==61 || token[i]==62 || token[i]==63 || token[i]==64) {
				printf("Erro léxico, linha %d: rótulo %s possui caractere inválido\n",(*cont).contadorlinha, token);
				j=1;
			}	
		}
		return j;
		
	}
	else {
		return (-1);
	}

}

/*****************************************************************************************************************************************
As funções abaixo são funções auxiliares que realizam a inclusão de um símbolo na tabela em questão. Para incluiTabelaSimbolos, além da tabela, do contador e do token com o símbolo a ser incluído, enviamos como argumento dois inteiros d e a. 'd' indica se o símbolo é definido ou não é 'a' indica o último endereço que esse símbolo foi visto (montador de uma passagem). Note que, mesmo se um símbolo não é definido o endereço é atualizado como o ponto do programa. Isso não faz muito diferença pois todos os símbolos tem seus endereços atualizados quando eles se tornam definidos. No caso de incluiTabelaSimbolo e incluiTabelaDefinicao, além da inclusão temos uma procura pela tabela, pois símbolos já definidos não podem ser incluídos, e, portanto, este erro deve ser tratados. Cada uma dessas funções retorna um ponteiro da sua respectiva estrutura já que a realocação atualizou isso. 
*****************************************************************************************************************************************/
tabsimb* incluiTabelaSimbolos (tabsimb* ts, contadores* cont, char* token, int d, int a) {
	
	int i, auxs;
	tabsimb* s;	

	for(i=0;i<(*cont).tamts;i++) { /*Verificação: o símbolo a ser incluído já existe e está definido na tabela de símbolos?*/
		if(!(strcmp(ts[i].simbolo, token)) && ts[i].definido == 1) {
			printf("Erro semântico, linha %d: símbolo %s já definido!\n", (*cont).contadorlinha, token);
			return ts;
		}
		if(!(strcmp(ts[i].simbolo, token)) && !ts[i].definido) { /*Se está na tabela de símbolos*/
			ts[i].definido = 1;				 /*mas não estava definido*/
			ts[i].endereco = (*cont).contadorinstr;		 /*ele passa a estar definido agora*/
			return ts;
		}		
	}	

	((*cont).tamts)++;    /*Aumentamos o tamanho da tabela de símbolos ao encontrar o rótulo*/
	auxs = (*cont).tamts -1;/*Valor total de elementos, tiramos um para saber a posição no vetor*/			
	s = (tabsimb*) realloc (ts, (*cont).tamts * (sizeof(tabsimb))); /*E realocamos o espaço para a tabela de símbolos*/
	strcpy(s[auxs].simbolo, token);/*E passamos os valores adequados para a TS*/
	s[auxs].simbolo[strlen(token)] = '\0';
	s[auxs].externo = 0; /*Obs.: AINDA NÃO FOI DEFINIDO SE O SÍMBOLO É EXTERNO OU NÃO!!!*/
 	s[auxs].endereco = (*cont).contadorinstr; 
 	s[auxs].definido = d;  /*O símbolo está definido para o caso de rótulos*/
 	s[auxs].lastaddr = a;  /*Aqui temos o último endereço em que ele foi visto, para o caso dos já definidos, -1*/
	return s;
}

tabdef* incluiTabelaDefinicao (tabdef* td, contadores* cont, char* token) {
	
	int i, auxd;
	tabdef* def;

	for(i=0;i<(*cont).tamtd;i++) { /*Verificação: o símbolo a ser incluído já existe na tabela de definições?*/
		if(!(strcmp(td[i].simbolo, token))) {
			printf("Erro semântico, linha %d: símbolo %s já definido como PUBLIC!\n", (*cont).contadorlinha, token);
			return td;
		}		
	}

	auxd = (*cont).tamtd;  		/*Posição no vetor que estará o novo símbolo (qtd-1)*/
	((*cont).tamtd)++;     		/*Atualizando a quantidade de símbolos*/
	def = (tabdef*) realloc (td, (*cont).tamtd*(sizeof(tabdef))); 
	strcpy(def[auxd].simbolo, token); /*símbolo incluído na tabela*/
	def[auxd].simbolo[strlen(token)] = '\0';
	def[auxd].endereco = (-1);   /*será atualizado no final, quando todos os endereços forem obtidos*/

	return def;
}

tabuso* incluiTabelaUso (tabuso* tu, contadores* cont, char* token) {
	
	int auxu;
	tabuso* uso;

	auxu = (*cont).tamtu;  /*Posição no vetor que estará o novo símbolo (qtd-1)*/
	((*cont).tamtu)++;     /*Atualizando a quantidade de símbolos*/
	uso = (tabuso*) realloc (tu, (*cont).tamtu*(sizeof(tabuso))); 
	strcpy(uso[auxu].simbolo, token); 		/*símbolo incluído na tabela*/
	uso[auxu].simbolo[strlen(token)] = '\0';
	uso[auxu].endcod = (*cont).contadorinstr;   /*o endereço em que ele é usado é aonde está o contador de instrução*/

	return uso;
}

/*****************************************************************************************************************************************
As funções abaixo são funções auxiliares que realizam a procura sequencial pelas tabelas enviadas como argumento. Elas retornam -1 se o símbolo não foi encontrado e retornam um valor >= 0 se o símbolo foi encontrado, o próprio retorno sendo a posição na tabela em que o símbolo está. Elas recebem como argumento as estruturas de suas respectivas tabelas e o token na qual queremos realizar a comparação. Para o caso de procuraTabelaSimbolos também enviamos como argumento a variável cont pois ela armazena o tamanho da tabela de símbolos que não é fixo durante o programa.  
*****************************************************************************************************************************************/
int procuraDiretiva(char* token, tabdir* dir) { /*Função para verificar se a diretiva em questão (se o token é alguma diretiva)*/

	int i,j=(-1);

	for(i=0;i<NDIR;i++) {				/*Para todos os 9 elementos na tabela*/
		j=strcmp(dir[i].diretiva,token);	/*Vemos se token é igual a uma delas*/
		if (j==0) { return i; }		/*Caso exista uma string igual retornamos a posição da diretiva na tabela p/ referência*/
	}

	return (-1); 	/*Caso não exista, retornamos um valor negativo para indicar isso*/
}			/*Retorna a posição na tabela se existe, e retorna -1 se não existe*/

int procuraInstrucao(char* token, tabinstr* instr) { /*Vê se o token é uma instrução*/

	int i,j=(-1);

	for(i=0;i<NINSTR;i++) {				/*Para todos os 9 elementos na tabela*/
		j=strcmp(instr[i].instrucao,token);	/*Vemos se token é igual a uma delas*/
		if (j==0) { return i; }		/*Caso exista uma string igual retornamos a posição da instrução na tabela p/ referência*/
	}

	return (-1); 	/*Caso não exista, retornamos um valor negativo para indicar isso*/
}


int procuraTabelaSimbolos (tabsimb* ts, contadores* cont, char* token) {
	
	int i;

	for(i=0;i<(*cont).tamts;i++) {
		if(!(strcmp(ts[i].simbolo, token))) {
			return i;
		}		
	}	

	return (-1);
}
/*****************************************************************************************************************************************
Função que analisa as situação de uma diretiva encontrada na seção de texto. De acordo com posdirtab temos a diretiva procurada. Se a diretiva é EXTERN, precisamos de um rótulo associado. Se não existe esse rótulo temos um erro. Se temos o rótulo, atualizamos o ponto na tabela de símbolos onde está esse rótulo, falando que o símbolo é externo. Se não é módulo não podemos ter essa diretiva, erro. Para BEGIN, temos um erro automático se a diretiva é encontrada: BEGIN deve ser definido antes da seção de texto. Indicamos o erro. Se já temos BEGIN ou se já temos END, indicamos o erro. Se não tem um rótulo, não temos como associar um nome ao módulo. Se temos BEGIN, informamos que é um módulo. Se a diretiva é END, também já temos um erro, dado que a diretiva END só deve aparecer no final da seção de dados. Se temos a diretiva SECTION, ela deve ser seguida por DATA. Se for, indicamos que a seção de texto acabou e indicamos a instrução em que isso acontece. Se não for DATA, temos uma seção inválida. Se o símbolo que segue não é uma palavra reservada ele é ignorado. Se a diretiva é CONST ou SPACE temos um erro, pois elas só devem ser incluídas na seção da dados. Os argumentos dessas diretivas são procurados e ignorados. Finalmente, se a diretiva for PUBLIC, pegamos o elemento que segue e vemos se ele é um símbolo e não uma diretiva, uma instrução ou um rótulo.Se é uma palavra reservada, PUBLIC está sem argumentos. Se não é uma palavra reservada e é um símbolo não externo, há a inclusão desse símbolo na tabela de definição. Se o símbolo encontrado é externo ele não pode ser público. Temos um erro. As outras diretivas (EQU e IF) não são tratadas aqui pois elas nem deveriam existir nessa parte da montagem.
Chamada por: analisaLinhaText
Chama: pulaEspacos, pegaToken, transformaMaiusculo (para pegar e arrumar tokens), procuraDiretiva, procuraInstrucao, verificaRotulo (para ter certeza que um símbolo é um símbolo válido), incluiTabelaDefinicao (para incluir símbolos públicos)
Argumentos: fonte (para pegar argumentos), temrot (para o caso de EXTERN e BEGIN que precisam ser precedidos por rótulo), posdirtab (posição da diretiva na tabela (pra identificar a diretiva)), ts (para a procura na tabela de símbolos no caso de verificação de símbolos externos já definidos), flg (para indicar erros, verificar se é módulo, se tem begin ou end, etc), td (abela de definições, é possível realizar inclusões nessa função), auxs (ponto da inclusão do último rótulo da tabela de símbolos, para se, caso seja EXTERN, possamos atualizar o ponto correto), cont (auxiliar), dir, instr (para verificação), sectionend (para indicar que atingimos a seção de dados)
Retorno: caso a tabela de definições seja realocada, retornamos o novo endereço.
*****************************************************************************************************************************************/
tabdef* trataDiretivaText(char* fonte, int temrot, int posdirtab, tabsimb* ts, flags* flg, tabdef* td, int auxs, contadores* cont, tabdir* dir, tabinstr* instr, int* sectionend) {

	int i, j, k, l, s;
	tabdef* def = td;
	char* token;
	char* token1;	
	if(posdirtab==EXTERN) {	/*Posição da diretiva EXTERN na tabela*/
		if(temrot >= 0) {		    /*Se tem um rótulo a diretiva está em um lugar válido*/
			ts[auxs].externo=1; 	    /*Se é essa a diretiva, atualizamos a tabela de símbolos*/
		}
		else {
			printf("Erro sintático, linha %d: nenhum rótulo associado à diretiva EXTERN\n", (*cont).contadorlinha);
			(*flg).erro++;
		}
		if(!(*flg).modulo) {
			printf("Erro semântico, linha %d: diretiva EXTERN em um programa que não é módulo\n", (*cont).contadorlinha);
			(*flg).erro++;		
		}
		(*cont).pontodeleitura = (*cont).pontodeleitura + strlen("EXTERN");
	}
	if(posdirtab==BEGIN) {
		(*flg).erro++;
		printf("Erro semântico, linha %d: BEGIN não pode ser definido dentro da seção de textos\n",(*cont).contadorlinha);
		if ((*flg).begin || (*flg).end) {
			printf("Erro semântico, linha %d: BEGIN ou END já definidos\n",(*cont).contadorlinha);
			(*flg).erro++;
		}
		if(temrot < 0) { /*Se não tem rótulo na linha não temos como associar um nome ao módulo!!!!!*/
			printf("Erro, linha %d: nenhum rótulo associado à diretiva BEGIN\n", (*cont).contadorlinha);
		}
		(*flg).modulo=1;   /*Se tem BEGIN, é módulo e precisamos de um END*/
		(*flg).begin++; /*Indicador de que eu encontrei o begin*/
		(*cont).pontodeleitura = (*cont).pontodeleitura + strlen("BEGIN");
	}
	if(posdirtab==END) {
		(*flg).erro++;
		printf("Erro semântico, linha %d: fim do módulo definido na seção de textos\n",(*cont).contadorlinha);
		if(!(*flg).begin) {
			printf("Erro semântico, linha %d: END sem BEGIN\n",(*cont).contadorlinha);
			(*flg).erro++;
		}
		if((*flg).end) {
			printf("Erro semântico, linha %d: END já definido!\n",(*cont).contadorlinha);
			(*flg).erro++;
		}
		(*flg).end++;
		(*cont).pontodeleitura = (*cont).pontodeleitura + strlen("END");
	}
	if(posdirtab==SECTION) {
		if(temrot >= 0) {		
			printf("Erro sintático, linha %d: rótulo precedendo a diretiva SECTION\n", (*cont).contadorlinha);
			(*flg).erro++;
		}
		if(!((*flg).modulo) && !((*flg).temstop)) {
			printf("Erro semântico, linha %d: SECTION inicializada mas não há STOP\n", (*cont).contadorlinha);
		}
		(*cont).pontodeleitura = (*cont).pontodeleitura + strlen("SECTION");
		pulaEspacos(fonte, cont);	/*verificação se o que segue SECTION é DATA (caso contrário está errado)*/
		token = pegaToken(fonte, cont);
		transformaMaiusculo(token, strlen(token));
		i = strcmp(token, "DATA");
		if (!i) { /*Se a seção incluída for SECTION DATA, acaba a análise de linhas para a parte TEXT*/
			*sectionend = (*sectionend) + 1;
			if((*flg).modulo==0 && (*flg).temstop==0) {
			    printf("Erro sintático, linha %d: o programa não é módulo e não tem instrução STOP\n", (*cont).contadorlinha);
			}
			(*cont).pontodeleitura = (*cont).pontodeleitura + strlen("DATA");		
		}
		else {  /*Caso a palavra que segue section não é DATA, ou SECTION está sem argumento, ou ele é inválido*/
			k = procuraDiretiva(token, dir);	   /*vemos se é uma diretiva*/
			l = procuraInstrucao(token,instr);	   /*vemos se é uma instrução*/
			j = verificaRotulo(token, cont);	   /*vemos se é um rótulo*/
				if(j<0 && l<0 && k<0) {		   /*se não é nenhum dos três precisamos ignorar esse símbolo inválido*/
					(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);
					printf("Erro sintático, linha %d: SECTION possui argumento inválido\n", (*cont).contadorlinha);	
				}
				else {
					printf("Erro sintático, linha %d: SECTION sem argumento\n", (*cont).contadorlinha);		
				}
		}
//		free(token);
	}
	if(posdirtab==CONST || posdirtab==SPACE) {
		printf("Erro sintático, linha %d: diretivas da seção de dados na seção de texto\n", (*cont).contadorlinha);		
		(*flg).erro++;
		(*cont).pontodeleitura = (*cont).pontodeleitura + strlen("SPACE");
		pulaEspacos(fonte, cont);	
		token = pegaToken(fonte, cont);
		transformaMaiusculo(token, strlen(token));
		i = atoi(token);
			if(i) { /*Se o argumento for um inteiro atualizamos o contador de posição de leitura mas ignoramos a inclusão*/
				(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token); /*do espaço/valor pois é um erro*/
				/*Fazemos nada depois pois o próximo token será lido normalmente*/
			}
//		free(token);
	}
	if(posdirtab==PUBLIC) {
		(*cont).pontodeleitura = (*cont).pontodeleitura + strlen("PUBLIC"); /*Atualizando o ponto de leitura*/	
		pulaEspacos(fonte, cont);	/*caso tenhamos essa diretiva, pegamos o próximo elemento*/
		token = pegaToken(fonte, cont);
		transformaMaiusculo(token, strlen(token)); /*arrumamos esse elemento*/
		i = procuraTabelaSimbolos(ts,cont,token);  /*vemos se ele existe na tabela de símbolos*/
		k = procuraDiretiva(token, dir);	   /*vemos se é uma diretiva*/
		l = procuraInstrucao(token,instr);	   /*vemos se é uma instrução*/
		j = verificaRotulo(token, cont);	   /*vemos se é um rótulo*/
		token1 = (char*) calloc ((strlen(token) + 2), sizeof(char));	
		strcpy(token1, token);			   /*pra eu não estragar token incluindo nele um ':'*/
		s = verificaRotulo(strcat(token1, ":"), cont); /*vemos se é um símbolo válido*/
		token1[strlen(token1)] = '\0';
		if(s>=0 && k==(-1) && l==(-1) && j==(-1)) { 
		/*Símbolo inválido (mas não é outro tipo de token) ou símbolo válido*/
			/*CASO 1 - ele não existe ou ele existe e não é externo*/
			if (i<0 || (i>=0 && ts[i].externo != 1)) { /*Preciso incluir na tabela de definições*/
				def = incluiTabelaDefinicao(td,cont,token);
				td = def;
				(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);
			}		
			/*CASO 2 - ele existe mas é externo*/
			else if (i>=0 && ts[i].externo == 1) {
				printf("Erro semântico, linha %d: símbolo %s definido como PUBLIC e EXTERN\n", (*cont).contadorlinha, token);
				(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);
			}
		}
		else { /*Caso tenhamos um caractere especial, temos que possivelmente uma nova linha começou. É necessário analisá-la*/
			printf("Erro sintático, linha %d: diretiva PUBLIC não apresenta seus argumentos\n", (*cont).contadorlinha);
			(*flg).erro++;
		}
//		free(token);
//		free(token1);
	}

	return def; /*retornamos a tabela de definições possivelmente atualizada*/

}
/*****************************************************************************************************************************************
Essa função tem como propósito encontrar tokens '+' e verificar se os argumentos que seguem são válidos para quando temos símbolos. Pegamos o primeito token que segue o símbolo e vemos se é um caractere de soma. Se não é, não há nada a se fazer, a função se encerra informando que não houve erros. Caso seja o caractere de soma, pegamos o próximo token, que precisa ser um número positivo. Caso essa função tenha sido chamada pela função trataCOPY (como o primeiro argumento de COPY), porém, temos que tratar esse segundo argumento de forma diferente, pois ele estará seguido de uma vírgula. Se tivermos essa situação, precisamos ignorar essa vírgula antes de fazer qualquer análise. Se essa função é chamada do copy e o último elemento não é uma vírgula, há um erro sintático, apesar de que o valor ainda será analisado. Se o elemento que segue o '+' é um número POSITIVO, retornamos esse valor para a função que chamou para que o atributo soma do objeto seja atualizado. Caso contrário, se ele é negativo, temos um erro. Se ele é igual à zero, ou o número é zero ou temos uma string. Se for zero, temos uma redundância. Se for uma string temos um erro. 
Chamada por: analisaInstrucaoText, trataCOPY
Chama: pegaToken, pulaEspacos (para pegar tokens)
Argumentos: fonte (objeto da nossa análise), cont (para atualizarmos (linhas)), flg (atualizado em caso de erro), copy (para indicar se devemos esperar uma vírgula como último caractere do argumento da soma. Para o caso onde há mais de um operando))
Retorno: 0 -> não somar nada, não houve erro significativo
       > 0 -> o quanto deve-se somar
       > 0 -> erro (provavelmente a soma não é seguida de um número e sim de uma string)
*****************************************************************************************************************************************/
int procuraSomaVetor(char* fonte, contadores* cont, flags* flg, int copy) {

	int i;
	char* token;
	char* token1;
	pulaEspacos(fonte, cont);			
	token = pegaToken(fonte, cont);
	if (!strcmp(token, "+")) {	/*Vemos se o token é o elemento de soma*/
		(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token); /*Se for, atualizamoso ponto de leitura e procuramos*/
		pulaEspacos(fonte, cont);					 /*o operando do elemento de soma*/
//		free(token);		
		token = pegaToken(fonte, cont);
			if(copy) {
				if(token[strlen(token)-1] == ',') {
					token1 = (char*) calloc ((strlen(token)+1),sizeof(char));					
					memmove(token1, token, strlen(token)-1);
					token1[strlen(token)] = '\0';
					i = atoi(token1); 
				}
				else {
				     printf("Erro sintático, linha %d: primeiro argumento não seguido de vírgula\n", (*cont).contadorlinha);
				     	i = atoi(token); /*Vemos se esse operando pode ser transformado em inteiro*/
				}
			}
			else {
				i = atoi(token);
			}
		if (i>0) {			/*Se puder, atualizamos esse ponto de leitura e retornamos o valor*/
			(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);
		} 
		else if (i<0) { /*Se é um valor negativo, vai contra as regras da linguagem*/
			printf("Erro sintático, linha %d: número negativo associado ao operador de soma\n", (*cont).contadorlinha);
			((*flg).erro)++;
			(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);
			/*Ainda assim, atualizamos o contador pois isso não deve mais ser avaliado pois ele é o possível operando*/
			return 0;
		}
		else {	/*Se é zero, ou o valor é zero, ou o token é uma string*/
			printf("Erro sintático, linha %d: operador de soma sem operando ou com operando redundante\n", (*cont).contadorlinha);
			((*flg).erro)++;	/*Apenas indicamos o erro e teremos que avaliar esse token novamente*/	
		}
//		free(token);
		return i;
	}
	else {
//		free(token);
		return 0;
	}
}

void trataCOPY(char* fonte, objeto* obj, contadores* cont, panalise* retorno, flags* flg, tabsimb* ts, tabuso* tu, tabinstr* instr, tabdir* dir, int posinstrtab) {

	int k,l,j,h,s;
	char* token;
	char* token1;

	pulaEspacos(fonte, cont); /*Caso em que não há nada somando,*/		
	token = pegaToken(fonte, cont); /*ou que a soma foi inserida sem espaço (token errado)*/
	transformaMaiusculo(token, strlen(token));
		if(token[(strlen(token)-1)] == ',') { /*Se o último elemento do token é uma vírgula*/
			token1 = (char*) calloc ((strlen(token)+1), sizeof(char));					
			memmove(token1, token, strlen(token)-1); /*Vemos o que é o token*/
			token1[strlen(token)] = '\0';
			k = procuraDiretiva(token1, dir);
			l = procuraInstrucao(token1,instr);	   	
			s = verificaRotulo(strcat(token1, ":"),cont);
			if(k<0 && l<0) { /*Se ele não é uma palavra reservada*/
				if(s>=0) {	/*e é um símbolo*/
					(*flg).erro = (*flg).erro + s;
					free(token1);
					token1 = (char*) calloc ((strlen(token)+1), sizeof(char));					
					memmove(token1, token, strlen(token)-1);
					token1[strlen(token)] = '\0';
					h=procuraTabelaSimbolos (ts, cont, token1);
					if (h>=0) {	/*Se o símbolo já existe, existem 2 possibilidades*/
						if(!ts[h].definido) { /*Se o símbolo não é definido, precisamos atualizar a lista*/
							k = ts[h].lastaddr;	
							ts[h].lastaddr = (*cont).contadorinstr;
							obj[(*cont).contadorinstr].codigo = k;
						}
						else {  /*Se ele é definido, precisamos apenas colocar o valor no objeto*/
							obj[(*cont).contadorinstr].codigo = ts[h].endereco;
						}
						if(ts[h].externo) { /*Se o símbolo é externo, precisamos incluí-lo na tabela de usos*/
							(*retorno).tu = incluiTabelaUso(tu, cont, token1);
							tu = (*retorno).tu;
						}
					}
					else {  /*Se não está na tabela, precisamos incluir como não definido*/				
						(*retorno).ts = incluiTabelaSimbolos(ts,cont,token1,0,(*cont).contadorinstr);
						ts = (*retorno).ts;
						obj[(*cont).contadorinstr].codigo = (-1);
					}
				}
			}
			else {
				printf("Erro sintático, linha %d: operando é palavra reservada\n",(*cont).contadorlinha);
				(*flg).erro++;	
				obj[(*cont).contadorinstr].codigo = 0;
				obj[(*cont).contadorinstr].somar = 0;		
			}
			obj[(*cont).contadorinstr].relativo = 1; /*indicamos que o símbolo é um endereço relativo*/
			obj[(*cont).contadorinstr].linha = (*cont).contadorlinha; /*colocamos a linha em que apareceu*/
			(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);
			((*cont).contadorinstr)++;
		}
		else {	/*Se o último elemento do primeiro token não é uma vírgula*/			
			k = procuraDiretiva(token, dir); /*Vemos o que é o token*/
			l = procuraInstrucao(token,instr);	   	
			j = verificaRotulo(token, cont);
			token1 = (char*) calloc ((strlen(token)), sizeof(char));
			strcpy(token1, token);			
			s = verificaRotulo(strcat(token1, ":"), cont);
				if(k>=0 || l>=0 || j>=0) {
					printf("Erro sintático, linha %d: instrução COPY sem argumentos\n",(*cont).contadorlinha);
					(*flg).erro++;
				//	free(token1);
				//	free(token);
					return;
				}
				else {
					if(s>=0) {	/*Sinalizamos para o erro*/
						(*flg).erro = (*flg).erro + s;
					}
					h=procuraTabelaSimbolos (ts, cont, token);
					if (h>=0) {			/*Se o símbolo já existe, existem 2 possibilidades*/
						if(!ts[h].definido) { /*Se o símbolo não é definido, precisamos atualizar a lista*/
							k = ts[h].lastaddr;	
							ts[h].lastaddr = (*cont).contadorinstr;
							obj[(*cont).contadorinstr].codigo = k;
						}
						else {  /*Se ele é definido, precisamos apenas colocar o valor no objeto*/
							obj[(*cont).contadorinstr].codigo = ts[h].endereco;
						}
						if(ts[h].externo) { /*Se o símbolo é externo, precisamos incluí-lo na tabela de usos*/
							(*retorno).tu = incluiTabelaUso(tu, cont, token);
							tu = (*retorno).tu;
						}
					}
					else {  /*Se não está na tabela, precisamos incluir como não definido*/
						(*retorno).ts = incluiTabelaSimbolos(ts,cont,token,0,(*cont).contadorinstr);
						ts = (*retorno).ts;						
						obj[(*cont).contadorinstr].codigo = (-1);
					}
					obj[(*cont).contadorinstr].relativo = 1; /*indicamos que o símbolo é um endereço relativo*/
					(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);
					k = procuraSomaVetor(fonte, cont, flg,1);
					obj[(*cont).contadorinstr].somar = k; /*Colocamos a soma no objeto para referências futuras*/
					obj[(*cont).contadorinstr].linha = (*cont).contadorlinha; /*colocamos a linha em que apareceu*/
					((*cont).contadorinstr)++; /*atualizando...*/
					//free(token);
					//free(token1);
				}

		}

	pulaEspacos(fonte, cont);	/*Segundo argumento de COPY*/		
	token = pegaToken(fonte, cont);
	transformaMaiusculo(token, strlen(token));
	k = procuraDiretiva(token, dir);	   	/*vemos se é uma diretiva*/
	l = procuraInstrucao(token,instr);	   	/*vemos se é uma instrução*/
	j = verificaRotulo(token, cont);	   	/*vemos se é um rótulo*/
	token1 = (char*) calloc ((strlen(token) + 1), sizeof(char));					
	strcpy(token1, token);			
	s = verificaRotulo(strcat(token1, ":"), cont); 	/*vemos se é um símbolo válido*/
		if(k>=0 || l>=0 || j>=0) {			/*Se é uma palavra reservada...*/
			printf("Erro sintático, linha %d: faltam operandos para a instrução %s\n", (*cont).contadorlinha,             					instr[posinstrtab].instrucao);
		}
		else {			/*Se é um token, temos que o token pode estar errado ou não*/
			if(s>=0) {	/*Sinalizamos para o erro*/
				(*flg).erro = (*flg).erro + s;
			}
			h=procuraTabelaSimbolos (ts, cont, token);
			if (h>=0) {	/*Se o símbolo já existe, existem 2 possibilidades*/
				if(!ts[h].definido) { /*Se o símbolo não é definido, precisamos atualizar a lista*/
					k = ts[h].lastaddr;	
					ts[h].lastaddr = (*cont).contadorinstr;
					obj[(*cont).contadorinstr].codigo = k;
				}
				else {  /*Se ele é definido, precisamos apenas colocar o valor no objeto*/
					obj[(*cont).contadorinstr].codigo = ts[h].endereco;
				}
				if(ts[h].externo) { /*Se o símbolo é externo, precisamos incluí-lo na tabela de usos*/
					(*retorno).tu = incluiTabelaUso(tu, cont, token);
					tu = (*retorno).tu;
				}
			}
			else {  /*Se não está na tabela, precisamos incluir como não definido*/
				(*retorno).ts = incluiTabelaSimbolos(ts,cont,token,0,(*cont).contadorinstr);
				ts = (*retorno).ts;				
				obj[(*cont).contadorinstr].codigo = (-1);
			}
			obj[(*cont).contadorinstr].relativo = 1; /*indicamos que o símbolo é um endereço relativo*/
			(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);
			k = procuraSomaVetor(fonte, cont, flg, 0);
			obj[(*cont).contadorinstr].somar = k; /*Colocamos a soma no objeto para referências futuras*/
			obj[(*cont).contadorinstr].linha = (*cont).contadorlinha; /*colocamos a linha em que apareceu*/
			((*cont).contadorinstr)++; /*atualizando...*/
		}
//		//free(token);
//		//free(token1);
}

/*****************************************************************************************************************************************
Essa função tem como propósito tratar as instruções que encontramos na seção de texto. Assim que chamamos a função já atualizamos o objeto incluindo nele o opcode da instrução encontrada. Instruções não são relativas. Então, a partir do número de operandos da instrução, definimos como iremos tratá-la. Para o caso de 2 operandos temos apenas o COPY. O COPY é tratado em outra função pois ele é muito grande. Para as instruções com um operando, procuramos o token que segue a instrução. Se esse token for uma palavra reservada ou um rótulo, quer dizer que a instrução está sem seus operandos (assumimos que a palavra reservada indica uma nova linha). Se o que segue a instrução é um símbolo (avaliada como um rótulo pois as regras são as mesmas. Incluímos no símbolo os : para que ele fique válido como rótulo), precisamos ver qual o seu estado: ele pode já ter sido incluído na tabela de símbolos ou não . Se ele foi incluído e ele está definido, apenas atualizamos o objeto com o endereço do símbolo. Se ele não está definido, atualizamos a lista de endereços em que o símbolo aparece (colocamos o endereço atual como o último endereço da lista e colocamos no objeto o último endereço visto que estava na tabela de símbolos). Se o símbolo é externo, precisamos atualizar a tabela de uso. Se o símbolo não está na tabela de símbolos, precisamos incluí-lo como não definido. A última possíbilidade para a instrução é ela não ter argumentos, ou seja, ser um STOP. Para esse caso não é necessário fazer nada, apenas atualizar a flag que indica que existe o STOP (usado em outras funções). Para o caso em que temos 1 operando, temos que antes de retornar, verifica-se se há uma operação de acesso à vetor seguindo o símbolo.
Chamada por: analisaLinhaText
Chama: pulaEspacos, pegaToken, TransformaMaiusculo (para tratar tokens), procuraDiretiva, procuraInstrucao, verificaRotulo (para verificar os casos em que não foi incluído um operando), procuraTabelaSimbolos, incluiTabelaSimbolos (para tratar o operando quando ele é um símbolo), procuraSomaVetor (para o caso de vetores)
Argumentos: fonte (para poder recuperar os operandos), obj (o vetor objeto é atualizado com opcodes e endereços), cont (usado como referência e atualizado aqui), retorno (para retornar as realocações feitas na tabela de símbolos e na tabela de uso), flg (para indicar erros ou para indicar que o STOP foi encontrado), ts (usada para procura e para enviar para a função de inclusão), tu (usada para a inclusão de símbolos externos), instr, dir (usadas como referência na hora de procurar palavras reservadas), posinstrtab (define a posição da instrução na tabela de instruções (opcode-1))
*****************************************************************************************************************************************/
void trataInstrucaoText(char* fonte, objeto* obj, contadores* cont, panalise* retorno, flags* flg, tabsimb* ts, tabuso* tu, tabinstr* instr, tabdir* dir, int posinstrtab) {

	int l,s,j,h, k=0;	
	char* token;
	char* token1;

	if((*cont).contadorinstr < 216-(instr[posinstrtab].operandos + 1)) { /*Pra não dar segmentation fault*/
		obj[(*cont).contadorinstr].codigo = posinstrtab+1; /*colocando o código da instrução no objeto*/
		obj[(*cont).contadorinstr].somar = 0;	      /*não precisamos somar nada à esse valor, é uma instrução*/
		obj[(*cont).contadorinstr].relativo = 0;	      /*não é relativo*/
		obj[(*cont).contadorinstr].linha = (*cont).contadorlinha; /*colocamos a linha em que apareceu*/
		((*cont).contadorinstr)++;
		(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(instr[posinstrtab].instrucao);	
		/*OBS.: A CORRETUDE DO VALOR DE SOMA SERÁ TRATADA DEPOIS!!!*/
		if(instr[posinstrtab].operandos == 2) { /*Analise COPY separadamente pois tem 2 argumentos*/
			trataCOPY(fonte, obj, cont, retorno, flg, ts, tu, instr, dir, posinstrtab);				
		} 
		if(instr[posinstrtab].operandos == 1) { /*Analise de todas as instruções menos COPY e STOP*/
			pulaEspacos(fonte, cont);	/*Pegando o token que segue a instrução*/	
			token = pegaToken(fonte, cont);
			transformaMaiusculo(token, strlen(token));
			k = procuraDiretiva(token, dir);	   	/*vemos se é uma diretiva*/
			l = procuraInstrucao(token,instr);	   	/*vemos se é uma instrução*/
			j = verificaRotulo(token, cont);	   	/*vemos se é um rótulo*/
			token1 = (char*) calloc ((strlen(token) + 2), sizeof(char));					
			strcpy(token1, token);			
			s = verificaRotulo(strcat(token1, ":"), cont); 	/*vemos se é um símbolo válido*/
			token1[strlen(token1)] = '\0';
			if(k>=0 || l>=0 || j>=0) {			/*Se é uma palavra reservada...*/
				printf("Erro sintático, linha %d: faltam operandos para a instrução %s\n", (*cont).contadorlinha,      						instr[posinstrtab].instrucao);
			}
			else {			/*Se é um token, temos que o token pode estar errado ou não*/
				if(s>=0) {	/*Sinalizamos para o erro*/
					(*flg).erro = (*flg).erro + s;
				}
				h=procuraTabelaSimbolos (ts, cont, token);
				if (h>=0) {	/*Se o símbolo já existe, existem 2 possibilidades*/
					if(!ts[h].definido) { /*Se o símbolo não é definido, precisamos atualizar a lista*/
						k = ts[h].lastaddr;	
						ts[h].lastaddr = (*cont).contadorinstr;
						obj[(*cont).contadorinstr].codigo = k;
					}
					else {  /*Se ele é definido, precisamos apenas colocar o valor no objeto*/
						obj[(*cont).contadorinstr].codigo = ts[h].endereco;
					}
					if(ts[h].externo) { /*Se o símbolo é externo, precisamos incluí-lo na tabela de usos*/
						(*retorno).tu = incluiTabelaUso(tu, cont, token);
						tu = (*retorno).tu;
					}
				}
				else {  /*Se não está na tabela, precisamos incluir como não definido*/
					(*retorno).ts = incluiTabelaSimbolos(ts,cont,token,0,(*cont).contadorinstr);
					ts = (*retorno).ts;
					obj[(*cont).contadorinstr].codigo = (-1);
				}
				obj[(*cont).contadorinstr].relativo = 1; /*indicamos que o símbolo é um endereço relativo*/
				(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);
				k = procuraSomaVetor(fonte, cont, flg,0);
				obj[(*cont).contadorinstr].somar = k; /*Colocamos a soma no objeto para referências futuras*/
				obj[(*cont).contadorinstr].linha = (*cont).contadorlinha; /*colocamos a linha em que apareceu*/
				(*cont).contadorinstr = (*cont).contadorinstr + instr[posinstrtab].operandos; /*atualizando...*/
			}
			//free(token);
			//free(token1);
		}
		else { /*Analise do STOP separadamente pois tem 0 argumentos*/
			((*flg).temstop)++; 			  /*Podemos indicar que existe um STOP no programa*/
		}      /*Caso existisse outra instrução com 0 operandos, eu colocaria um if. Mas como não tem, não vou gastar espaço*/
	}
	else {
		printf("Erro: o seu programa excede o tamanho máximo (216 endereços de memória)\n");
	}
}



/*****************************************************************************************************************************************
Analisa a parte da seção de textos LINHA POR LINHA. Espera-se que cada linha na seção de textos tenha a estrutura <rótulo>:(<diretiva>|<instrução>)<operandos>. Pegamos então o primeiro token e vemos se é um rótulo. Se é um rótulo ele deve ser incluído na tabela de símbolos, pois, assume-se que é a sua primeira definição. Pegamos o próximo token. Procura-se uma diretiva ou uma instrução independente de existir um rótulo, já que ele não precisa estar presente em uma linha. Portanto, encontrando um rótulo ou não, procuramos por uma diretiva. Se fizermos a comparação com a tabela e encontrarmos uma diretiva válida, temos que tratar essa diretiva de acordo com essa seção. Se não encontramos uma diretiva, procuramos uma instrução. Se for uma instrução, tratamos essa instrução de acordo com a seção de texto. Caso não tenhamos nem uma instrução, nem uma diretiva, é um símbolo qualquer. Ele pode ser o início de uma nova linha (rótulo, diretiva ou instrução) ou ele pode ser um símbolo sem significado. No segundo caso ignoramos atualizando o contador de ponto de leitura.
Chamada por: monta
Chama: pulaEspacos, pegaToken, transformaMaiusculo (para pegar e arrumar tokens), verificaRotulo (para ver se temos um rótulo), procuraTabelaDiretivas, procuraTabelaInstrucoes (para verificar se um token é uma diretiva ou uma instrução), incluiTabelaSimbolos (para incluir rótulos encontrados)
Argumentos: fonte (para analisar a seção de texto), cont (para referência e para ser atualizado (contador de linhas e de programa, por exemplo)), flg (para indicar erros e enviar para outras funções), ts (para que seja possível procurar e atualizar), dir, instr (para que seja possível procurar palavras reservadas), td (enviada como argumento para outra função dentro dessa), obj (atualizado quando tratamos instruções), tu (atualizada quando temos símbolos definidos como externos) 
Retorno: retornamos uma estrutura que permite que eu retorne todas as atualizações em todas as tabelas (já que todas elas podem ser possivelmente realocadas)
*****************************************************************************************************************************************/
panalise analisaLinhaText(char* fonte, contadores* cont, flags* flg, tabsimb* ts, tabdir* dir, tabdef* td, tabinstr* instr, objeto* obj, tabuso* tu) {

	int r, d, i;
	char* token;
	char* token1;
	panalise retorno;	
	
	retorno.sectionendT = 0; /*inicializando (para o caso de não haver realloc)*/
	retorno.ts = ts;
	retorno.td = td;
	retorno.tu = tu;

 	pulaEspacos(fonte, cont);	
	token = pegaToken(fonte, cont);
	transformaMaiusculo(token, strlen(token));
	r=verificaRotulo(token,cont);
		if(r>=0) {		      		/*O primeiro elemento da linha é um rótulo*/
			((*flg).erro) = (*flg).erro + r;/*Caso o rótulo esteja errado, precisamos comunicar o FLAG de erro*/
			token1 = (char*) calloc ((strlen(token)+1), sizeof(char));					
			memmove(token1, token, strlen(token)-1); /*Eliminando os ':' do rótulo*/
			token1[strlen(token)] = '\0';			
			retorno.ts = incluiTabelaSimbolos(ts, cont, token1, 1, -1); /*O 1 indica que o símbolo entra já definido*/
			ts = retorno.ts;		/*Vai ser necessário atualizar o endereço que a tabela de símbolos aponta!!!*/
			(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token); 
			pulaEspacos(fonte, cont);	/*Próximo passo: pegar outros tokens*/		
			//free(token);			
			token = pegaToken(fonte, cont);	
			transformaMaiusculo(token, strlen(token));
		}	/*Não tem ELSE porque pode não ter rótulo (entrar direto na diretiva ou na instrução)*/
		
		d=procuraDiretiva(token,dir);

		if(d>=0) {		
			retorno.td = trataDiretivaText(fonte,r,d,ts,flg,td,((*cont).tamts)-1,cont,dir,instr,&(retorno.sectionendT));
			td = retorno.td;
		}
		else {  
			i=procuraInstrucao(token,instr);	/*Se o token não é diretiva, vemos se é uma instrução*/
			if(i>=0) {				/*Se for, tratamos isso*/
				trataInstrucaoText(fonte, obj, cont, &retorno, flg, ts, tu, instr, dir, i);
				ts = retorno.ts;
				tu = retorno.tu;
			}
			else {	/*Caso depois de um rótulo(existente ou não) não tenhamos uma instrução ou uma diretiva*/
				r = verificaRotulo(token, cont); /*Não é diretiva nem instrução porque eu já verifiquei*/
				if (r<0) {
					(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token); /*Esse símbolo é algo inválido*/
				}
				printf("Erro sintático, linha %d: linha não possui estrutura <rótulo>(<instrução>|<diretiva>)<operandos>\n", (*cont).contadorlinha);
			}							/*Ignoramos ele e seguimos em frente com a tradução*/
		}
	//free(token);	
	return retorno;
}

/*****************************************************************************************************************************************
Essa função analisa o que devemos fazer após encontrarmos uma diretiva na seção da dados. As três diretivas permitidas são SPACE, CONST e END. Ou seja, se não temos nenhum desses casos, temos uma diretiva inválida para seção e um erro. Isso é indicado para que o objeto não seja gerado. Agora, se a nossa diretiva é CONST ou SPACE, precisamos ver qual é o token que segue. Vemos se o token é um número. Para o caso de CONST, se o token é um número inteiro qualquer (e não uma string) podemos atualizar o objeto. Se ele é uma string, não interessa muito o que é essa string: falta um argumento para CONST. Assumimos que essa string é algum token da próxima linha. Se a diretiva for SPACE, precisamos ser mais cautelosos. O número que segue não pode ser negativo e não pode ser zero, mas SPACE pode não ter argumentos. Logo, se tivermos 0 ou um número negativo, indicamos erro. Se temos uma string, assumimos SPACE 1. Se temos um valor positivo, incluímos em uma quantidade do objeto igual ao tamanho definido o valor 0. Note que eu uso a variável 'relativo' do objeto para indicar o espaço disponível a partir daquele ponto do vetor para aquela variável. A variável não é relativa, mas o seu endereço é. Por isso, uso esse valor como referência para verificar que o usuário estourou o espaço do vetor. Não há problemas de imprimir esses endereços em RELATIVES no executável final pois os relativos são avaliados até termos SECTION DATA. Se não temos SPACE ou CONST, existe a possibilidade de termos END. END indica o final do módulo e portanto o final do programa. A variável end recebe essa informação. Caso tenhamos um END sem definição de módulo, temos um erro. Caso tenhamos um END que já apareceu, temos um erro. Caso tenhamos texto seguindo esse END, temos um erro. Para o caso em que temos uma diretiva inválida para a seção de dados, verificamos qual é a diretiva ou se temos uma instrução, para ignorarmos completamente essa diretiva/instrução e seus operandos.
Chamada por: analisaLinhaText
Chama: pulaEspacos, pegaToken, transformaMaiusculo (para tratar tokens), procuraDiretiva, procuraInstrucao, verificaRotulo (apenas para verificar e ignorar linhas inválidas)
Argumentos: fonte (para pegar tokens que seguem), obj (para atualizar o objeto no caso de SPACE e CONST), cont (para referência), flg (para indicar erros e verificar se é ou não um módulo), dir, instr (para referência na hora da procura), posdirtab (indica a posição da diretiva na tabela de diretivas para os condicionais), end (indica que encontramos END e por isso, acabou a seção)
*****************************************************************************************************************************************/
void trataDiretivaData(char* fonte, objeto* obj, contadores* cont, flags* flg, tabdir* dir, tabinstr* instr, int posdirtab, int* end) {

	int n,m,r,i,d;
	char* token;

	if(posdirtab == CONST || posdirtab == SPACE) {
		pulaEspacos(fonte, cont);	/*Se é CONST ou SPACE, precisamos pegar o token e ver se é um número*/
		token = pegaToken(fonte, cont);
		n = atoi(token); 		/*Vemos se o token é um inteiro*/
		if(posdirtab == CONST) {
			if(n || !strcmp(token,"0")) { /*Se o token é um número ou é o próprio zero*/
				if((*cont).contadorinstr < 216) {
					obj[(*cont).contadorinstr].codigo = n; /*atualizamos aquele */
					obj[(*cont).contadorinstr].somar = 0;  /*não é necessário somar nada*/
					obj[(*cont).contadorinstr].relativo = 0; /*não é relativo*/
					obj[(*cont).contadorinstr].linha = (*cont).contadorlinha; /*colocamos a linha em que apareceu*/
					((*cont).contadorinstr)++;
					(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token); /*atualizamos o ponto de leitura*/
				}				
				else {
					printf("Erro: o programa excede o número máximo de instruções\n");
					((*flg).erro)++;
				}
			}
			else { /*Caso contrário ele é uma string, está faltando o argumento de CONST*/
				printf("Erro sintático, linha %d: argumento faltando para a diretiva\n", (*cont).contadorlinha);
				((*flg).erro)++;
			}
		}
		else { /*Se for SPACE*/
			if(n<=0) { /*Se o valor for negativo, uma string ou o próprio zero, não vale*/
				if(n<0 || !strcmp(token,"0")) { /*Se o argumento for um NÚMERO inválido, ignoramos ele*/
					printf("Erro sintático, linha %d: argumento inválido para a diretiva\n", (*cont).contadorlinha);
					((*flg).erro)++;					
					(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);
				}
				else {
					if(n==0 && strcmp(token,"0")) { /*Se é um símbolo qualquer, assumimos SPACE 1*/
						obj[(*cont).contadorinstr].codigo = 0; 
						obj[(*cont).contadorinstr].somar = 0;
						obj[(*cont).contadorinstr].relativo = 1; 
						obj[(*cont).contadorinstr].linha = (*cont).contadorlinha;
						((*cont).contadorinstr)++;
					}	/*Ponto de leitura não incrementado pois assume-se que esse símbolo é da prox. linha*/
				}
			} /*Caso contrário, ele pode ser o elemento de uma próxima linha*/
			else {  /*Se o valor for positivo*/
				for(;n>0;n--) { /*Separamos o espaço*/
					if((*cont).contadorinstr < 216) {					
						obj[(*cont).contadorinstr].codigo = 0; 
						obj[(*cont).contadorinstr].somar = 0;
						obj[(*cont).contadorinstr].relativo = n; /*Pra eu saber quanto espaço tem, farei > 1*/
						obj[(*cont).contadorinstr].linha = (*cont).contadorlinha;
						((*cont).contadorinstr)++;
					}
					else {
						break;
					}
				}
				(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);
			}
		}
		//free(token);
	}
	else if (posdirtab == END) {
		if(!(*flg).modulo) {
			printf("Erro semântico, linha %d: diretiva END incluída em arquivo que não é módulo\n", (*cont).contadorlinha);
			((*flg).erro)++;
			(*end)--; /*Se não é um módulo, podemos continuar analisando. Como vou incrementar ali em baixo...*/
		}
		if(!(*flg).begin) {
			printf("Erro semântico, linha %d: diretiva END sem BEGIN\n", (*cont).contadorlinha);
			((*flg).erro)++;
		}
		if((*flg).end) {
			printf("Erro semântico, linha %d: diretiva END já incluída\n", (*cont).contadorlinha);
			((*flg).erro)++;
		}
		((*flg).end)++;
		(*end)++;
		pulaEspacos(fonte, cont);    				
		token = pegaToken(fonte, cont);
			if(strlen(token) > 0) {
				printf("Erro semântico, linha %d: código após fim do módulo\n", (*cont).contadorlinha);
			}
	}
	else {  /*Se a diretiva não é CONST ou SPACE temos que ignorar os argumentos (possíveis) que seguem*/
		printf("Erro sintático, linha %d: diretiva inválida para seção de dados\n", (*cont).contadorlinha);
		((*flg).erro)++;
			for(m=0;m<dir[posdirtab].operandos;m++) { /*Ignoramos os operandos*/
				pulaEspacos(fonte, cont);     /*Se eles não forem outros caracteres especiais*/	
				//free(token);				
				token = pegaToken(fonte, cont);
				transformaMaiusculo(token, strlen(token));
				d = procuraDiretiva(token, dir);	   /*vemos se é uma diretiva*/
				i = procuraInstrucao(token,instr);	   /*vemos se é uma instrução*/
				r = verificaRotulo(token, cont);	   /*vemos se é um rótulo*/
					if(d<0 && i<0 && r<0) {	/*Se não é nenhum dos três, podemos atualizar o ponto de leitura*/
						(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);
					}
					else {	/*Se não é um símbolo qualquer, então consideramos que é uma nova linha*/
						break;
					}
			}
	//	free(token);
	}

}

/*****************************************************************************************************************************************
Essa função analisa uma linha individual da seção de dados. Espera-se que uma linha da seção de dados tenha a forma: <rótulo>:SPACE<operando>|<rótulo>:CONST<operando>|END. Começamos pegando um token. Precisamos, antes de tudo, verificar se o token não é o '\0'. Se for esse o caso, chegamos ao final do arquivo, nenhuma análise deve ser feita. Caso contrário, deixamos o token todo maiúsculo e verificamos se ele é um rótulo e se ele é uma diretiva. Se ele é um rótulo, precisamos procurá-lo na tabela de símbolos. Se ele já está definido, temos um erro, pois o rótulo indica que ele deveria ser definido agora. Caso contrário, podemos encontrar o ponto na tabela de símbolos em que está o símbolo e o definimos. Se ele simplesmente não está incluído na tabela de símbolos, fazemos a inclusão. Pegamos então o próximo token, que deve ser uma diretiva. Essa diretiva é tratada em trataDiretivaData. Caso o primeiro token da linha não seja um rótulo, a diretiva será válida apenas se ela for o END, isso SE o programa for um módulo. Se esse primeiro símbolo não é um rótulo e nem uma diretiva, existe um símbolo inválido no meio da seção da dados. Note que da forma que a função foi arrumada, a seção de dados pode ficar vazia (não há avisos para seção de dados vazia).
Chamada por: monta
Chama: pulaEspacos, pegaToken, transformaMaiusculo (as três pegam e arrumam os tokens para análise), verificaRotulo (para ver se o símbolo é um rótulo), procuraDiretiva (para ver se o símbolo é uma diretiva), procuraTabeladeSimbolos (para saber o que fazer com possíveis símbolos encontrados), incluiTabelaSimbolos (para o caso de símbolos novos), trataDiretivaData (vai verificar se as diretivas encontradas estão de acordo com a seção), procuraInstrucao (apenas para informar símbolos inválidos)
Argumentos: fonte (o objeto da nossa análise), cont (usado como referência, sempre sendo atualizado), obj (o objeto é atualizado aqui, afinal, teremos diretivas CONST e SPACE, possivelmente), flg (para indicar erros), dir (tabela de diretivas para referência), instr (tabela de instruções para referência), ts (tabela de símbolos para referência e inclusão), end (marcador que irá indicar quando eu cheguei ao final da seção de dados (para o caso de módulos))
Retorno: a função retorna um ponteiro para a estrutura da tabela de símbolos, devido ao fato de que é possível haver a realocação do bloco de memória contendo a tabela de símbolos devido à possível inclusão de símbolos
*****************************************************************************************************************************************/
tabsimb* analisaLinhaData(char* fonte, contadores* cont, objeto* obj, flags* flg, tabdir* dir, tabinstr* instr, tabsimb* ts, int* end) {

	int r,d,s,k,x,y,z;	
	char* token;
	tabsimb* simb;

	simb = ts; /*Eu preciso inicializar simb. Se ts não for atualizada e eu não inicializar simb eu retorno LIXO!!!*/
	pulaEspacos(fonte, cont);	
	token = pegaToken(fonte, cont);

	if(strlen(token)>0) {			
		transformaMaiusculo(token, strlen(token));
		r = verificaRotulo(token, cont);
		d = procuraDiretiva(token, dir);
		if(r>=0) {	/*Se é um rótulo, vemos se ele já existe*/
			(*flg).erro = (*flg).erro + r;	
			token[strlen(token)-1] = '\0';
			s = procuraTabelaSimbolos(ts,cont,token);
			if (s>=0) { /*Se ele existe*/
				if(ts[s].definido) { /*Se ele já está definido, temos um erro*/
					printf("Erro semântico, linha %d: símbolo %s já definido\n", (*cont).contadorlinha, token);
					((*flg).erro)++;	
				}
				else { /*Se ele ainda não foi definido*/
					ts[s].definido=1; /*Indicamos que a partir de agora ele está definido*/
					ts[s].endereco=(*cont).contadorinstr; /*E atualizamos o endereço dele*/				
				}
				(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token) + 1;				
			}
			else { /*Se ele não existe*/
				simb=incluiTabelaSimbolos (ts, cont, token, 1, (-1)); /*Incluimos na tabela de símbolos*/
				ts = simb; 
				(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token) + 1;
			}				
				
				pulaEspacos(fonte, cont);	/*Pegamos o próximo elemento*/
				//free(token);				
				token = pegaToken(fonte, cont);
				transformaMaiusculo(token, strlen(token));
				d = procuraDiretiva(token,dir); /*Vemos se é uma diretiva*/
					if(d>=0) {  		/*Se é, analisamos a diretiva*/
						(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);			
						trataDiretivaData(fonte, obj, cont, flg, dir,instr,d, end);
					}
					else {  		/*Se não é, temos um erro (pois essa é a seção de dados)*/
						printf("Erro sintático, linha %d: argumento inválido para seção de dados\n",  							       (*cont).contadorlinha);
						((*flg).erro)++;  /*Ignoramos então esse símbolo e seguimos em frente atualizando*/
						(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);
					}
			
			//free(token);

		}
		else {  /*Se não é um rótulo*/
			if(d>=0 && d!=END) { /*E é uma diretiva*/
				printf("Erro sintático, linha %d: diretiva inválida para essa seção ou não associada à nenhum rótulo",	(*cont).contadorlinha);
				((*flg).erro)++;
				(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);	
					if(dir[d].operandos > 0) {
						for(k=0;k<dir[d].operandos;k++) {
							pulaEspacos(fonte, cont);     /*Se eles não forem outros caracteres especiais*/	
							//free(token);							
							token = pegaToken(fonte, cont);
							transformaMaiusculo(token, strlen(token));
							x = procuraDiretiva(token, dir);	   /*vemos se é uma diretiva*/
							y = procuraInstrucao(token,instr);	   /*vemos se é uma instrução*/
							z = verificaRotulo(token, cont);	   /*vemos se é um rótulo*/
							if(x<0 && y<0 && z<0) {
								(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);
							}
							else {	/*Se não é um símbolo qualquer, então consideramos que é uma nova linha*/
								break;
							}
						}	
					}
			}
			else if(d==END) {
				(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);			
				trataDiretivaData(fonte, obj, cont, flg, dir,instr,d, end);
			}
			else { /*Se não é um rótulo e não é uma diretiva*/
				printf("Erro sintático, linha %d: símbolo inválido na seção de dados\n", (*cont).contadorlinha);
				(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);
			}
			//free(token);
		}
	}

	return simb;
}
/*****************************************************************************************************************************************
Essa função é a função que atualiza os endereços de todas as variáveis criadas no programa além de verificar erros que não podem ser verificados em outros momentos (principalmente erros semânticos). Então, percorremos a tabela de símbolos para atualizar os endereços em que cada símbolo aparece no objeto. A lista é inserida na estrutura do objeto aonde o código normalmente ficaria. Vemos na tabela de símbolos o último endereço em que ele foi visto (lastaddr). A partir daí, vamos nessa posição do objeto e atualizamos o código que lá se encontra, somando ao valor de soma que pode vir a aparecer. Verificações feitas: vemos se a soma ultrapassa o limite permitido. Se o endereço é absoluto, não podemos percorrê-lo como um vetor. Se ele não for absoluto, precisamos ver o quanto podemos percorrer. Por isso usamos aux. Se o valor de soma do ponto em que estamos for maior do que o permitido, temos um erro de acesso à área inválida. Se um símbolo não é relativo, ele só pode ser constante, isso indica que, se a instrução que o precede é um DIV e a constante que ele representa é o valor 0, não podemos permitir tal divisão. Se a instrução anterior a ele for de PULO, precisamos indicar o erro de pulo à constante. Se a instrução que precede a instrução que o precede é um COPY, temos o valor de uma constante sendo alterado, o mesmo acontece se a instrução que o precede é um STORE. Vemos também se há um pulo para a seção de dados: instrução de pulo precedendo o nosso símbolo. Vemos se o símbolo está na seção de dados a partir do delimitador do fim da seção de texto. Esses são os erros analisados. Agora, se o símbolo nem foi encontrado na tabela de símbolos em primeiro lugar, temos o problema de um símbolo não definido. Ignoramos esse símbolo, pois, mesmo não escrevendo no arquivo destino, é necessário terminar de avaliar todos os casos, para que o usuário tenha acesso a todos os seus erros. 
Chamada por: monta
Chama: -
Argumentos: obj (o arquivo objeto é o que estará sendo analisado), ts (para verificar todos os símbolos), cont (usa seus valores como referência), flg (para indicar erros que surgem)
*****************************************************************************************************************************************/
void arrumaObjeto(objeto* obj, tabsimb* ts, contadores* cont, flags* flg, tabuso* tu) {

	int i, j, temp, aux;

	for(i=0;i<(*cont).tamts;i++) {	/*Para todos os elementos da tabela de símbolos*/
		if(ts[i].definido) {	/*Se ele foi definido vamos percorrer a lista de endereços onde ele aparece*/
			j = ts[i].lastaddr; /*Pegamos o último endereço do objeto em que ele aparece*/
			while(j!=(-1)) {    /*E enquanto não chegamos no -1*/
				temp = obj[j].codigo;	/*Colocamos no objeto o valor do endereço + soma*/
				obj[j].codigo = ts[i].endereco + obj[j].somar;
				
				if(obj[(ts[i].endereco)].relativo > 0) { /*Se o que tem no endereço é relativo, pode ser um vetor*/
					aux = (obj[(ts[i].endereco)].relativo - 1);
				}
				else {	/*se é absoluto, então não posso somar nada*/
					aux = 0;
					obj[j]. relativo = 0; /*E eu preciso informar que aquele endereço agora tem um endereço ABSOLUTO*/
				}			      /*SÍMBOLO absoluto só pode ser constante*/

				if(aux < obj[j].somar) { /*Se a soma ultrapassa o espaço do vetor*/
					printf("Erro semântico, linha %d: acesso à área inválida\n", obj[j].linha);
					((*flg).erro)++;
				}
				obj[j].somar = 0;	/*Soma já feita, atualizamos o valor de soma*/

				if(!obj[j].relativo) { /*Se um SÍMBOLO não é relativo, ele é constante*/
					/*Símbolo constante, endereço prévio é DIV (opcode = 4 e não é relativo)*/
					if(obj[(obj[j].codigo)].codigo == 0 && obj[j-1].codigo == 4 && !obj[j-1].relativo) {
						printf("Erro semântico, linha %d: divisão por 0 (constante %s)\n", obj[j].linha, ts[i].simbolo);
						((*flg).erro)++;
					}
					/*Símbolo constante, endereço prévio é algum JUMP (opcode = 5-8 e não é relativo)*/
					if((obj[j-1].codigo>4 && obj[j-1].codigo<9) && !obj[j-1].relativo) { 
						printf("Erro semântico, linha %d: pulo para constante %s\n", obj[j].linha, ts[i].simbolo);
						((*flg).erro)++;
					}
					/*Símbolo constante, 2 endereços prévios é COPY (opcode = 10 e não é relativo)*/
					if(obj[j-2].codigo == 9 && !obj[j-2].relativo) { /*COPY*/
						printf("Erro semântico, linha %d: valor da constante %s sendo alterado\n",obj[j].linha, ts[i].simbolo);
						((*flg).erro)++;
					}
					/*Símbolo constante, endereço prévio é STORE ou INPUT (opcode = 11/12 e não é relativo)*/
					if((obj[j-1].codigo == 11 || obj[j-1].codigo == 12) && !obj[j-1].relativo) { 
						printf("Erro semântico, linha %d: valor da constante %s sendo alterado\n", obj[j].linha, ts[i].simbolo);
						((*flg).erro)++;
					} 
				}
				if((obj[j-1].codigo>4 && obj[j-1].codigo<9 && !obj[j-1].relativo) && obj[j].codigo>=(*flg).fimtext) {
					printf("Erro semântico, linha %d: pulo para seção de dados\n", obj[j].linha);
					((*flg).erro)++;
				}
				j = temp;	/*analisado esse endereço, podemos pegar o próximo*/
			}
		}
		else {	/*Se o símbolo não está definido ainda, temos um problema. Ignoramos e passamos para o próximo*/
			printf("Erro semântico: símbolo %s não definido\n", ts[i].simbolo);
			((*flg).erro)++;
		}
	}
	for(i=0;i<(*cont).tamtu;i++) {	/*arrumando a soma para o caso de vetores externos*/
		j = tu[i].endcod;
		obj[j].codigo = obj[j].codigo + obj[j].somar;
	}

}

/*****************************************************************************************************************************************
Para todos os elementos na tabela de definições, vemos se existe um equivalente na tabela de símbolo. A tabela de definição armazena os símbolos públicos. Aqui, devemos associar esses símbolos ao seu endereço real no código. Como o montador é de 1 passagem, podemos não ter os endereços definitivos até que se encerre a análise de todas as partes do código. Para um símbolo na td que possui um igual definido na ts, atualizamos o seu endereço. Para símbolos definidos na tabela de definições mas não definidos na tabela de símbolos temos um erro: símbolo não definido (símbolos públicos pertencem ao módulo em questão, eles precisam ser definidos!). Existe uma outra condição que mostra erro se o símbolo nem foi encontrado na tabela de símbolos, mas a princípio isso não deve acontecer, a não ser que o programa tenha um erro. Essa parte do condicional foi utilizada para testes.
Chamada por: monta
Chama: procuraTabelaSimbolos (para procurar os símbolos)
Argumentos: ts (para que possa se fazer a pesquisa), td (a tabela de definições é o elemento principal dessa função, cont (usado como referência. Tem o tamanho da tabela de símbolos para que não haja acesso à áreas indevidas), flg (se temos um erro, precisamos atualizar essa estrutura))
*****************************************************************************************************************************************/
void arrumaTabDef(tabsimb* ts, tabdef* td, contadores* cont, flags* flg) { /*Colocando os endereços na tabela de definições*/

	int i, j;
	char* simbolo;

	for(i=0;i<(*cont).tamtd;i++) { /*Para todos os elementos na tabela de definições*/
		simbolo = (char*) calloc (strlen(td[i].simbolo) + 1, sizeof(char)); 
		strcpy(simbolo, td[i].simbolo);
		j = procuraTabelaSimbolos(ts,cont,simbolo); /*Procuramos o símbolo em TD na TS*/
		if(j>=0) {  /*Se ele existe*/
			if(ts[j].definido) { /*E está definido, podemos atualizar TD*/	
				td[i].endereco = ts[j].endereco;
			}
			else { /*Mas não foi definido, não podemos atualizar TD*/
				printf("Erro semântico: símbolo público %s não definido\n", td[i].simbolo);
				((*flg).erro)++;
			}
		}
		else { /*Se o símbolo nem foi encontrado na TS então (apesar de que isso não deve acontecer, mas enfim...)*/
			printf("Erro semântico: símbolo público %s não definido\n", td[i].simbolo);
			((*flg).erro)++;
		}
	}
}

/*****************************************************************************************************************************************
Essa função tem o propósito de gerar o arquivo objeto, ou o executável, caso não seja um módulo. Tentamos abrir o arquivo e se não for possível, informamos ao usuário. Se for possível, temos duas possibilidades: se for um módulo, precisamos incluir no arquivo destino a tabela de uso, a tabela de definição, os endereços relativos e o código. Caso não seja módulo, precisamos apenas incluir o código. Para incluir a tabela de uso, percorremos a tabela e imprimos todos os símbolos e endereços em que eles aparecem. Para imprimir a tabela de definições fazemos o mesmo. Os endereços relativos estão definidos no próprio objeto, então, percorremos o objeto até o número de instrução que temos (armazenado em cont (a estrutura de contadores)) vendo, para cada endereço, se ele é relativo ou não. Finalmente, imprimimos o código, percorrendo a estrutura objeto mais uma vez.
Chamada por: monta
Chama: -
Argumentos: obj (código objeto/executável), td, tu (tabelas para referência), arquivo (nome do arquivo em que será escrito o código), cont (usado como referência), flg (para identificar se é um módulo ou não)
*****************************************************************************************************************************************/
void printObjeto(objeto* obj, tabdef* td, tabuso* tu, char* arquivo, contadores* cont, flags* flg) { 	
	
	FILE* fp;
	int i;
	char* narq = (char*) calloc (sizeof(char), (3+strlen(arquivo)));

	strcpy(narq, arquivo);
	if((*flg).modulo) { strcat(narq, ".o"); }
	else { strcat(narq, ".e"); }

	fp = fopen(narq, "w+");
	
	if(fp==NULL) {
		printf("\nImpossível gerar o arquivo objeto. Não há espaço disponível\n");
	}
	else {
		if((*flg).modulo) {
			fprintf(fp, "TABLE USE\n");
			for(i=0;i<(*cont).tamtu;i++) {
				fprintf(fp, "%s %d\n", tu[i].simbolo, tu[i].endcod);
			}
			fprintf(fp,"\n");
			fprintf(fp, "TABLE DEFINITION\n");
			for(i=0;i<(*cont).tamtd;i++) {
				fprintf(fp, "%s %d\n", td[i].simbolo, td[i].endereco);
			}
			fprintf(fp,"\n");
			fprintf(fp, "RELATIVES\n");
			for(i=0;i<(*flg).fimtext;i++) {
				if(obj[i].relativo>0) { /*O endereço do símbolo é relativo na seção DATA*/			
					fprintf(fp, "%d ", i); /*Mas o que tem lá dentro não é!*/
				}	/*Por isso, se é relativo, mas está fora da seção de texto, não interessa*/			
			}		/*O relativo pode ser > 0 fora da seção de texto pq eu uso ele pra marcar a qtd de SPACE*/
			fprintf(fp,"\n\n");
			fprintf(fp, "CODE\n");
		}
		for(i=0;i<(*cont).contadorinstr;i++) {	
			if(obj[i].codigo < 10 && obj[i].codigo >= 0 ) {
				fprintf(fp, "0");
			}	
			fprintf(fp, "%d ", obj[i].codigo);
		}
		fprintf(fp,"\n");
	}
	
	fclose(fp);
	free(narq);	
}
/*****************************************************************************************************************************************
Essa função é a função que começa o processo de montagem após o pré-processamento. Ela cria a tabela de instruções e a tabela de diretivas para referência, e ela cria as estruturas da tabela de símbolos, da tabela de definição e da tabela de uso. É claro que, inicialmente, elas começam vazias. Dois vetores auxiliares, contadores e flags também são criados pois eles serão utilizados para o acompanhamento da leitura do programa e para a identificação de erros. O vetor objeto também é criado, seu tamanho máximo sendo 216 por definição do Assembly inventado. A estrutura panalise é uma estrutura utilizada apenas para o retorno de funções quando é necessário retornar mais de um valor (vulgo, a estrutura de retorno de reallocs para a ts, a td e a tu). Com tudo inicializado, a primeira coisa a se fazer é procurar SECTION TEXT ou alguma definição de módulo seguido de SECTION TEXT. Essa é a função primeirosPassos(). Depois, temos um loop: enquanto não termina a seção de textos (determinado pela linha SECTION DATA) ou enquanto o arquivo fonte ainda pode ser lido, analisamos o programa, linha por linha. A cada análise de linha atualizamos o valor de ts, td e tu, pois pode ter ocorrido uma alocação ou realocação dentro dessa função caso tenha-se encontrado um novo símbolo (externo, público ou nenhum). Se o arquivo chegar ao final sem uma seção de dados, indicamos um erro. Depois, analisamos a seção de dados enquanto o arquivo não termina ou enquanto não é definido um END, para o caso de um módulo. O valor da tabela de símbolos é atualizado sempre que lemos uma linha. Depois de analisadas todas as seções, precisamos arrumar o código objeto, pois esse montador é o de 1 passagem. Precisamos atualizar os endereços para cada símbolo em TS. Depois, com os símbolos definidos, arrumamos a tabela de definição. Finalmente, se houver algum erro, não chamamos a função que imprime no arquivo. Se não houver erros, podemos chamar a função que imprime o objeto no arquivo. Liberamos a memória alocada.
Chamada por: main
Chama: montaTabelaInstrucoes, montaTabelaDiretivas, inicializaContadores, inicializaFlags, primeirosPassos, analisaLinhaText, analisaLinhaData, arrumaObjeto, arrumaTabDef, printObjeto
Argumentos: fonte (arquivo com o código após pré-processamento), arquivo(nome do arquivo destino)
*****************************************************************************************************************************************/

void monta(char* fonte, char* arquivo, int erropreproc) {
	
	tabinstr* instr;
	tabdir* dir;
	tabsimb* ts = (tabsimb*) calloc (1,sizeof(tabsimb));
	ts[0].simbolo[0] = '\0';
	tabdef* td = (tabdef*) calloc (1,sizeof(tabdef));
	td[0].simbolo[0] = '\0';
	tabuso* tu = (tabuso*) calloc (1,sizeof(tabuso));
	tu[0].simbolo[0] = '\0';
	contadores cont;
	flags flg;
	objeto* obj = (objeto*) calloc (216,sizeof(obj));
	panalise retorno;
	instr = montaTabelaInstrucoes();
	dir = montaTabelaDiretivas();
	inicializaContadores(&cont);
	inicializaFlags(&flg);
	if(erropreproc>=0) {
		flg.erro = erropreproc;		/*apresenta os erros do pre-processamento*/
	}
//	inicializaObjeto(obj); não precisa, to fazendo calloc
	retorno.ts = ts;
	retorno.td = td;
	retorno.tu = tu;
	retorno.sectionendT = 0;
	retorno.sectionendD = 0;
	flg.erro = flg.erro + primeirosPassos(fonte, &cont, ts, &retorno, &flg);
		while(!retorno.sectionendT && fonte[cont.pontodeleitura] != '\0') { 
			retorno = analisaLinhaText(fonte, &cont, &flg, ts, dir, td, instr, obj, tu);
			ts = retorno.ts;
			td = retorno.td;
			tu = retorno.tu;
		}
		flg.fimtext = cont.contadorinstr;
		if(!retorno.sectionendT && fonte[cont.pontodeleitura] == '\0') {
			printf("Erro sintático, linha %d: não há seção de dados\n", cont.contadorlinha);
		}
		else {
			while(fonte[cont.pontodeleitura] != '\0' || !retorno.sectionendD) {
				ts = analisaLinhaData(fonte, &cont, obj, &flg, dir, instr, ts, &(retorno.sectionendD));
			}
		}
	arrumaObjeto(obj, ts, &cont, &flg, tu);
	arrumaTabDef(ts, td, &cont, &flg);
		if(flg.erro == 0) {
			printObjeto(obj, td, tu, arquivo, &cont, &flg);
		}
		else {
			printf("\nImpossível montar arquivo objeto. Por favor, corrija os erros e tente novamente\n");
		}

	free(instr);
	free(dir);
	free(td);
	free(tu);
	free(ts);
	free(obj);

}
/*TESTES*********************************************************************************************************************************/
/****************************************************************************************************************************************/

int main(int argc, char* argv[]) {

      FILE *fp;
      char token[255];
      char* codigo_apos_pre_processamento;
      int tamanho_para_alocar;
      char* arquivoL;
      char* arquivoE;
      int filenamesize;
      contadores cont;
      flags flgs;

      if (argc<2) {
	  printf("\nPrograma chamado com o número errado de argumentos. Por favor, tente novamente\n\n");
      }
      else {
	      filenamesize = strlen(argv[1]);
	      arquivoL = (char*) calloc (filenamesize + 5, sizeof(char));
	      arquivoE = (char*) calloc (filenamesize + 1, sizeof(char));
	      strcpy(arquivoL,argv[1]);
	      strcat(arquivoL, ".asm");
	      strcpy(arquivoE,argv[1]);
	      arquivoE[filenamesize] = '\0';

	      inicializaContadores(&cont);
	      inicializaFlags(&flgs);

	      fp = fopen(arquivoL, "r");
	      if (fp == NULL) {
		   printf("Arquivo inexistente! Impossível montar. Por favor, tente novamente com outro arquivo.\n");
	      }
	      else {
	      /* Tamanho máximo do codigo_apos_pre_processamento.*/
		      fseek(fp, 0L, SEEK_END);
		      tamanho_para_alocar = ftell(fp);
		      rewind(fp);
		      codigo_apos_pre_processamento = (char*) calloc(tamanho_para_alocar,sizeof(char));   	      
		      if (fp) {
			  pre_processador(fp, token, codigo_apos_pre_processamento, &cont, &flgs);
			  fclose(fp);
	      	      }

		      monta(codigo_apos_pre_processamento, arquivoE, flgs.erro);

		      free(arquivoL);
		      free(arquivoE);
	      	      free(codigo_apos_pre_processamento);
	      }
      }
      return 0;
 
}


/****************************************************************************************************************************************/
/****************************************************************************************************************************************/
