#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sb.h"

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
/*Protótipos******************************************************************************************************************************/
typedef struct contadores contadores; /*K*/
typedef struct objeto objeto;
typedef struct flags flags;	      /*K*/
typedef struct tabdef tabdef;
typedef struct tabsimb tabsimb;
typedef struct tabuso tabuso; 
typedef struct tabdir tabdir;	      /*K*/
typedef struct tabinstr tabinstr;     /*K*/
typedef struct panalise panalise;
tabinstr* montaTabelaInstrucoes();    /*K*/
tabdir* montaTabelaDiretivas();       /*K*/
void pulaEspacos(char* fonte, contadores* cont);  /*K*/
void transformaMaiusculo (char* token, int comprimento); /*K*/
void inicializaContadores (contadores* cont); /*K*/
void inicializaFlags (flags* flg);  /*K*/
char* pegaToken(char* fonte, contadores* cont); /*K*/
int encontraSectionText(char* fonte, contadores* cont); 
int verificaRotulo(char* token, contadores* cont);
tabsimb* incluiTabelaSimbolos (tabsimb* ts, contadores* cont, char* token, int d, int a);
int procuraDiretiva(char* token, tabdir* dir); /*K*/
int procuraInstrucao(char* token, tabinstr* instr); /*K*/
int procuraTabelaSimbolos (tabsimb* ts, contadores* cont, char* token);
tabdef* trataDiretivaText(char* fonte, int temrot, int posdirtab, tabsimb* ts, flags* flg, tabdef* td, int auxs, contadores* cont, tabdir* dir, tabinstr* instr, int* sectionend);
tabdef* incluiTabelaDefinicao (tabdef* td, contadores* cont, char* token); 
int procuraSomaVetor(char* fonte, contadores* cont, flags* flg, int copy); 
panalise analisaLinhaText(char* fonte, contadores* cont, flags* flg, tabsimb* ts, tabdir* dir, tabdef* td, tabinstr* instr, objeto* obj, tabuso* tu); /*ESTÁ COM ALGUM ERRO!*/
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

	ti = (tabinstr*) malloc(14*(sizeof(tabinstr))); /*Alocando os 14 espaços da tabela, já que temos 14 instruções*/

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

/*Tabela de Diretivas-TESTADO(1)**********************************************************************************************************/
/*****************************************************************************************************************************************/

struct tabdir {		  /*Armazena as diretivas para referências futuras na hora de traduzir*/
	char diretiva[8]; /*A maior referência é SECTION que tem 7 letras. O oitavo espaço no vetor existe para armazenar o '\0'*/
	int operandos;	  /*Número de "operandos" da diretiva*/
	int tamanho;	  /*Espaço em memória ocupado pela diretiva*/
};

typedef struct tabdir tabdir;

tabdir* montaTabelaDiretivas() {

	tabdir* td;

	td = (tabdir*) malloc(9*(sizeof(tabdir)));

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

/*Structs de Tabelas**********************************************************************************************************************/
/*****************************************************************************************************************************************/

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
};

typedef struct contadores contadores;

struct objeto {		/*Vetor que irá representar o código objeto*/
	int codigo;	/*Quando criado, provavelmente terá 216 posições (definido como o máximo de endereços de memória disponível)*/
	int somar;	/*Para o caso de diretivas X+2, preciso saber o quanto preciso somar*/
	int relativo;	/*Define se o endereço é relativo ou não*/
};

typedef struct objeto objeto;

struct flags {
	int erro;
	int modulo;
	int temstop;
	int begin;
	int end;
};

typedef struct flags flags;

struct panalise {			/*Struct para lidar com retornos de novos ponteiros caso eu precise atualizar mais*/
	tabsimb* ts;			/*de uma tabela em analisaLinhaText*/
	tabdef* td;
	tabuso* tu;
	int sectionend;			/*indicador de quando precisamos parar de ler linhas de TEXT (p/ o LOOP de chamada)*/
};					/*de analisaLinhaText*/

typedef struct panalise panalise;
/*****************************************************************************************************************************************/
/*****************************************************************************************************************************************/

/*Funções Auxiliares**********************************************************************************************************************/
/*****************************************************************************************************************************************/

/*TESTADO(2)*/
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


/*TESTADO(2)*/
void transformaMaiusculo (char* token, int comprimento) { /*TESTADO*/
	int i; /*variável auxiliar do loop*/

	for(i=0;i<comprimento;i++) {	            /*para cada elemento da string vemos se ele está em letra minúscula*/
		if(token[i]>=97 && token[i]<=122) { /*ou seja, possui o ascii entre 97 = 'a' e 122 = 'z'*/
			token[i] = token[i] - 32;   /*Se tivermos um desses casos, subtraímos 32 para obter a versão maiúscula da letra*/
		}				    /*Os outros ascii não precisam ser incluídos pois a questão maiúscula-minúscula*/
	}					    /*só envolve LETRAS*/
}

/*TESTADO*/
void inicializaContadores (contadores* cont) {	     /*Função auxiliar para zerar os contadores (no início são todos zero)*/
	
	(*cont).pontodeleitura = 0;
	(*cont).contadorlinha = 0;
	(*cont).contadorinstr = 0;
	(*cont).tamts = 0;
	(*cont).tamtd = 0;
	(*cont).tamtu = 0;
}

/*TESTADO*/
void inicializaFlags (flags* flg) {

	(*flg).erro = 0;
	(*flg).modulo = 0;
	(*flg).temstop = 0;
	(*flg).begin = 0;
	(*flg).end = 0;

}
/****************************************************************************************************************************************/
/****************************************************************************************************************************************/




/*Funções Montador (1a (e única) passagem))**********************************************************************************************/
/****************************************************************************************************************************************/

char* pegaToken(char* fonte, contadores* cont) {

	int i = (*cont).pontodeleitura, j=(*cont).pontodeleitura;
	char* token;
	while(fonte[i] != '\n' && fonte[i] != '\t' && fonte[i] != ' ' && fonte[i] != '\r') { i++; }
	i = i - j;
	token = (char*) malloc(i*sizeof(char) + 1); /*A CULPA É DESTE FILHO DA PUTA*/
	i = 0;
	while(fonte[j] != '\n' && fonte[j] != '\t' && fonte[j] != ' ' && fonte[j] != '\r') {
		token[i] = fonte[j];
		i++;
		j++;
	}
	token[i] = '\0';
	return token;
}


int encontraSectionText(char* fonte, contadores* cont) { /*A primeira coisa que precisamos fazer é encontrar a seção SECTION TEXT*/

	char* token;

	pulaEspacos(fonte,cont); /*Pulamos os espaços iniciais no texto*/
	token = pegaToken (fonte,cont); /*Pegamos o token que aparece depois desse primeiro espaço*/
	transformaMaiusculo(token,strlen(token));
		if (strcmp(token,"SECTION") != 0) {
			printf("Erro sintático, linha %d: seção SECTION TEXT ausente ou com erro.\n", (*cont).contadorlinha);
			//(*cont).pontodeleitura = (*cont).pontodeleitura - strlen(token);
			return 1;
		}
		else {
			(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);
			free(token);
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
					free(token);
					return 0;
				}
		}

}

int verificaRotulo(char* token, contadores* cont) { /*Retorna -1 se não é rótulo, retorna 0 se é, retorna 1 se é e existe algum erro*/

	int i = strlen(token) - 1, j=0;
	
	if (token[i]==':') {
		if (strlen(token)>101) {
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
	memmove(s[auxs].simbolo, token, strlen(token));/*E passamos os valores adequados para a TS*/
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
	uso[auxu].endcod = (*cont).contadorinstr;   /*o endereço em que ele é usado é aonde está o contador de instrução*/

	return uso;
}

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

tabdef* trataDiretivaText(char* fonte, int temrot, int posdirtab, tabsimb* ts, flags* flg, tabdef* td, int auxs, contadores* cont, tabdir* dir, tabinstr* instr, int* sectionend) {

	int i, j, k, l,s;
	char* token;
	char* token1;	
	if(posdirtab==EXTERN) {	/*Posição da diretiva EXTERN na tabela*/
		if(temrot >= 0) {		    /*Se tem um rótulo a diretiva está em um lugar válido*/
			ts[auxs].externo=1; 	    /*Se é essa a diretiva, atualizamos a tabela de símbolos*/
		}
		else {
			printf("Erro, linha %d: nenhum rótulo associado à diretiva EXTERN\n", (*cont).contadorlinha);
		}
		(*cont).pontodeleitura = (*cont).pontodeleitura + strlen("EXTERN");
	}
	if(posdirtab==BEGIN) {
		if ((*flg).begin || (*flg).end) {
			printf("Erro sintático, linha %d: BEGIN ou END já definidos!\n",(*cont).contadorlinha);
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
		if(!(*flg).begin) {
			printf("Erro sintático, linha %d: END sem BEGIN\n",(*cont).contadorlinha);
			(*flg).erro++;
		}
		if((*flg).end) {
			printf("Erro sintático, linha %d: END já definido!\n",(*cont).contadorlinha);
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
			printf("Erro sintático, linha %d: SECTION inicializada mas não há STOP\n", (*cont).contadorlinha);
		}
		if((*flg).modulo && !((*flg).end)) {
			printf("Erro sintático, linha %d: SECTION inicializada mas não há END\n", (*cont).contadorlinha);
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
			if((*flg).modulo==1 && (*flg).end==0) {
			    printf("Erro sintático, linha %d: o programa não tem diretiva END\n", (*cont).contadorlinha);
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
		free(token);
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
		free(token);
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
		token1 = (char*) malloc ((strlen(token) + 1)*sizeof(char));		
		strcpy(token1, token);			   /*pra eu não estragar token incluindo nele um ':'*/
		s = verificaRotulo(strcat(token1, ":"), cont); /*vemos se é um símbolo válido*/
		if(s>=0 && k==(-1) && l==(-1) && j==(-1)) { 
		/*Símbolo inválido (mas não é outro tipo de token) ou símbolo válido*/
			/*CASO 1 - ele não existe ou ele existe e não é externo*/
			if (i<0 || (i>=0 && ts[i].externo != 1)) { /*Preciso incluir na tabela de definições*/
				td = incluiTabelaDefinicao(td,cont,token);
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
		free(token);
		free(token1);
	}

	return td; /*retornamos a tabela de definições possivelmente atualizada*/

}

int procuraSomaVetor(char* fonte, contadores* cont, flags* flg, int copy) {

	int i;
	char* token;
	char* token1;
	pulaEspacos(fonte, cont);			
	token = pegaToken(fonte, cont);
	if (!strcmp(token, "+")) {	/*Vemos se o token é o elemento de soma*/
		(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token); /*Se for, atualizamoso ponto de leitura e procuramos*/
		pulaEspacos(fonte, cont);					 /*o operando do elemento de soma*/
		free(token);		
		token = pegaToken(fonte, cont);
			if(copy) {
				if(token[strlen(token)-1] == ',') {
					token1 = (char*) malloc ((strlen(token))*sizeof(char));					
					memmove(token1, token, strlen(token)-1);
					i = atoi(token1); 
				}
				else {
				     printf("Erro sintático, linha %d: primeiro argumento não seguido de vírgula", (*cont).contadorlinha);
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
			printf("Erro sintático, linha %d: operador de soma sem operando\n", (*cont).contadorlinha);
			((*flg).erro)++;	/*Apenas indicamos o erro e teremos que avaliar esse token novamente*/	
		}
		free(token);
		return i;
	}
	else {
		free(token);
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
			token1 = (char*) malloc ((strlen(token))*sizeof(char));					
			memmove(token1, token, strlen(token)-1); /*Vemos o que é o token*/		
			k = procuraDiretiva(token1, dir);
			l = procuraInstrucao(token1,instr);	   	
			s = verificaRotulo(strcat(token1, ":"),cont);
			if(k<0 && l<0) { /*Se ele não é uma palavra reservada*/
				if(s>=0) {	/*e é um símbolo*/
					(*flg).erro = (*flg).erro + s;
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
						free(token1);
						token1 = (char*) malloc ((strlen(token))*sizeof(char));					
						memmove(token1, token, strlen(token)-1);						
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
			(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);
			((*cont).contadorinstr)++;
		}
		else {	/*Se o último elemento do primeiro token não é uma vírgula*/			
			k = procuraDiretiva(token, dir); /*Vemos o que é o token*/
			l = procuraInstrucao(token,instr);	   	
			j = verificaRotulo(token, cont);
			token1 = (char*) malloc ((strlen(token))*sizeof(char));
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
	token1 = (char*) malloc ((strlen(token) + 1)*sizeof(char));					
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
			((*cont).contadorinstr)++; /*atualizando...*/
		}
//		//free(token);
//		//free(token1);
}

void trataInstrucaoText(char* fonte, objeto* obj, contadores* cont, panalise* retorno, flags* flg, tabsimb* ts, tabuso* tu, tabinstr* instr, tabdir* dir, int posinstrtab) {

	int l,s,j,h, k=0;	
	char* token;
	char* token1;

	if((*cont).contadorinstr < 216-(instr[posinstrtab].operandos + 1)) { /*Pra não dar segmentation fault*/
		obj[(*cont).contadorinstr].codigo = posinstrtab+1; /*colocando o código da instrução no objeto*/
		obj[(*cont).contadorinstr].somar = 0;	      /*não precisamos somar nada à esse valor, é uma instrução*/
		obj[(*cont).contadorinstr].relativo = 0;	      /*não é relativo*/
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
			token1 = (char*) malloc ((strlen(token) + 1)*sizeof(char));					
			strcpy(token1, token);			
			s = verificaRotulo(strcat(token1, ":"), cont); 	/*vemos se é um símbolo válido*/
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
					}
				}
				else {  /*Se não está na tabela, precisamos incluir como não definido*/
					(*retorno).ts = incluiTabelaSimbolos(ts,cont,token,0,(*cont).contadorinstr);
					obj[(*cont).contadorinstr].codigo = (-1);
				}
				obj[(*cont).contadorinstr].relativo = 1; /*indicamos que o símbolo é um endereço relativo*/
				(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);
				k = procuraSomaVetor(fonte, cont, flg,0);
				obj[(*cont).contadorinstr].somar = k; /*Colocamos a soma no objeto para referências futuras*/
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





panalise analisaLinhaText(char* fonte, contadores* cont, flags* flg, tabsimb* ts, tabdir* dir, tabdef* td, tabinstr* instr, objeto* obj, tabuso* tu) {

	int r, d, i;
	char* token;
	char* token1;
	panalise retorno;	
	
	retorno.sectionend = 0;
 	pulaEspacos(fonte, cont);	
	token = pegaToken(fonte, cont);
	transformaMaiusculo(token, strlen(token));
	r=verificaRotulo(token,cont);
		if(r>=0) {		      		/*O primeiro elemento da linha é um rótulo*/
			((*flg).erro) = (*flg).erro + r;/*Caso o rótulo esteja errado, precisamos comunicar o FLAG de erro*/
			token1 = (char*) malloc ((strlen(token))*sizeof(char));					
			memmove(token1, token, strlen(token)-1); /*Eliminando os ':' do rótulo*/			
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
			retorno.td = trataDiretivaText(fonte,r,d,ts,flg,td,((*cont).tamts)-1,cont,dir,instr,&(retorno.sectionend));
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
				printf("Erro sintático, linha %d: linha não possui estrutura <rótulo><instrução>|<diretiva><operandos>\n", 						(*cont).contadorlinha);
				(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token); /*Esse símbolo é algo inválido*/
			}							/*Ignoramos ele e seguimos em frente com a tradução*/
		}
	//free(token);	
	return retorno;
}

/****************************************************************************************************************************************/
/****************************************************************************************************************************************/

void trataDiretivaData(char* fonte, objeto* obj, contadores* cont, flags* flg, tabdir* dir, tabinstr* instr, int posdirtab) {

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
				printf("Erro sintático, linha %d: argumento inválido para a diretiva\n", (*cont).contadorlinha);
				((*flg).erro)++;
					if(n<0 || !strcmp(token,"0")) { /*Se o argumento for um NÚMERO inválido, ignoramos ele*/
						(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);
					}
			} /*Caso contrário, ele pode ser o elemento de uma próxima linha*/
			else {  /*Se o valor for positivo*/
				for(;n>0;n--) { /*Separamos o espaço*/
					if((*cont).contadorinstr < 216) {					
						obj[(*cont).contadorinstr].codigo = 0; 
						obj[(*cont).contadorinstr].somar = 0;
						obj[(*cont).contadorinstr].relativo = 0;
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


tabsimb* analisaLinhaData(char* fonte, contadores* cont, objeto* obj, flags* flg, tabdir* dir, tabinstr* instr, tabsimb* ts) {

	int r,d,s,k,x,y,z;	
	char* token;
	tabsimb* simb;

	pulaEspacos(fonte, cont);	
	token = pegaToken(fonte, cont);
	transformaMaiusculo(token, strlen(token));
	r = verificaRotulo(token, cont);
	d = procuraDiretiva(token, dir);
		if(r>=0) {	/*Se é um rótulo, vemos se ele já existe*/
			(*flg).erro = (*flg).erro + r;			
			s = procuraTabelaSimbolos(ts,cont,token);
			if (s>=0) { /*Se ele existe*/
				if(ts[s].definido) { /*Se ele já está definido, temos um erro*/
					printf("Erro semântico, linha %d: símbolo já definido", (*cont).contadorlinha);
					((*flg).erro)++;	
				}
				else { /*Se ele ainda não foi definido*/
					ts[s].definido=1; /*Indicamos que a partir de agora ele está definido*/
					ts[s].endereco=(*cont).contadorinstr; /*E atualizamos o endereço dele*/				
				}
			}
			else { /*Se ele não existe*/
				simb=incluiTabelaSimbolos (ts, cont, token, 1, (-1)); /*Incluimos na tabela de símbolos*/
				ts = simb; 
				(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);				
				pulaEspacos(fonte, cont);	/*Pegamos o próximo elemento*/
				//free(token);				
				token = pegaToken(fonte, cont);
				transformaMaiusculo(token, strlen(token));
				d = procuraDiretiva(token,dir); /*Vemos se é uma diretiva*/
					if(d>=0) {  		/*Se é, analisamos a diretiva*/
						(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);			
						trataDiretivaData(fonte, obj, cont, flg, dir,instr,d);
					}
					else {  		/*Se não é, temos um erro (pois essa é a seção de dados)*/
						printf("Erro sintático, linha %d: argumento inválido para seção de dados",  							       (*cont).contadorlinha);
						((*flg).erro)++;  /*Ignoramos então esse símbolo e seguimos em frente atualizando*/
						(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);
					}
			}
			//free(token);

		}
		else {  /*Se não é um rótulo*/
			if(d>=0) { /*E é uma diretiva*/
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
			else { /*Se não é um rótulo e não é uma diretiva*/
				printf("Erro sintático, linha %d: símbolo inválido na seção de dados\n", (*cont).contadorlinha);
				(*cont).pontodeleitura = (*cont).pontodeleitura + strlen(token);
			}
			//free(token);
		}

	return simb;
}



/*TESTES*********************************************************************************************************************************/
/****************************************************************************************************************************************/

int main() {
	int i;
	tabinstr* instr;
	tabdir* dir;
	tabsimb* ts = (tabsimb*) malloc (sizeof(tabsimb));
	tabdef* td = (tabdef*) malloc (sizeof(tabdef));
	tabuso* tu = (tabuso*) malloc (sizeof(tabuso));
	contadores cont;
	flags flg;
	char fonte[200];
	objeto obj[216];
	panalise retorno;
	instr = montaTabelaInstrucoes();
	dir = montaTabelaDiretivas();
	inicializaContadores(&cont);
	inicializaFlags(&flg);
//	strcpy(fonte, "ROT: SPACE 3 ");
//	ts = analisaLinhaData(fonte, &cont, obj, &flg, dir, instr, ts);
//	printf("\nPonto de leitura: %d\n",cont.pontodeleitura);
//	printf("\nPonto de programa: %d\n", cont.contadorinstr);
//	for(i=0;i<cont.contadorinstr;i++) {
//		printf(" %d ", obj[i].codigo);
//	}
//	printf("\n");
//	printf("\nTabela Símbolos: %s %d %d %d %d\n", ts[0].simbolo, ts[0].definido, ts[0].externo, ts[0].lastaddr, ts[0].endereco);
	strcpy(fonte, "TAB: COPY A + 2, C + 3");
	retorno = analisaLinhaText(fonte, &cont, &flg, ts, dir, td, instr, obj, tu);
	ts = retorno.ts;
	td = retorno.td;
	printf("\nPonto de leitura: %d\n",cont.pontodeleitura);
	printf("\nPonto de programa: %d\n", cont.contadorinstr);
	printf("\nTabela Símbolos: %s %d %d %d %d\n", ts[0].simbolo, ts[0].definido, ts[0].externo, ts[0].lastaddr, ts[0].endereco);
	printf("\nTabela Símbolos: %s %d %d %d %d\n", ts[1].simbolo, ts[1].definido, ts[1].externo, ts[1].lastaddr, ts[1].endereco);
	printf("\nTabela Símbolos: %s %d %d %d %d\n", ts[2].simbolo, ts[2].definido, ts[2].externo, ts[2].lastaddr, ts[2].endereco);
//	printf("\nTabela Definição: %s %d\n", td[0].simbolo, td[0].endereco);
	for(i=0;i<cont.contadorinstr;i++){
		printf("(%d,%d,%d)",obj[i].codigo,obj[i].somar,obj[i].relativo);
	}
	printf("\n\n");
	free(dir);
	free(instr);
	free(ts);	
	return 0;

}


/****************************************************************************************************************************************/
/****************************************************************************************************************************************/
