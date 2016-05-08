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

/*
* typedef das structs
*/
typedef struct codigo_objeto codigo_objeto;
typedef struct contadores contadores;
typedef struct tabela_symbol_value tabela_symbol_value;

/*
* Assinatura das funcoes
*/
codigo_objeto* criar_vetor_codigo(FILE* fp, codigo_objeto* codigo, contadores* cont);
void inicializa_contador(contadores* cont);
void criar_tabela_def(FILE* fp, tabela_symbol_value* tab_def, contadores* cont, int fator_de_correcao);
void criar_tabela_uso(FILE* fp, tabela_symbol_value* tab_uso, contadores* cont, int fator_de_correcao);
int* criar_novos_relativos(FILE* fp, int fator_de_correcao, contadores* cont, int* relativos);
void atualizar_tabela_relativos(int* relativos, tabela_symbol_value* tab_uso, contadores* cont);
codigo_objeto* juntar_objetos(codigo_objeto* codigo_a, codigo_objeto* codigo_b, int tamanho_a, int tamanho_b);
void atualizar_objetos_tabela_relativos(int* relativos, codigo_objeto* codigo,contadores* cont, int fator_de_correcao_b);
int atualizar_objeto_uso_def (codigo_objeto* codigo, tabela_symbol_value* tab_uso, tabela_symbol_value* tab_def, contadores* cont);
void gerar_executavel(codigo_objeto* codigo_concatenado, int tamanho_total, char* executavel);

/*
* Definicoes das structs
*/
struct codigo_objeto {
	int valor;
};

struct contadores {
	int indice_cod_obj;
	int indice_tab_def;
	int indice_tab_uso;
	int indice_relativo;
	int indice_fator_correcao;
};

struct tabela_symbol_value {
	char *symbol;
	int value;
};


/* 
* Funcao principal. Recebe argumentos que sao os nomes dos arquivos utilizados, cria tabelas de Uso, de Definição, contadores,  
* lista de enderecos relativos. 
*	
* Parametrs
* 	argc - numero de argumentos
*	argv - argumentos recebidos via linha de comando.
*/
int main (int argc, char *argv[]){
	if(argc != 4){
		printf("\nNúmero de argumentos incorreto. Deve-se incluir o nome dos arquivos do primeiro módulo, do segundo módulo e do "
		"arquivo executável. \n\n");
		return 1;
	}

	char* executavel = (char*) calloc(1, sizeof(argv[3])); 
	strcpy(executavel, argv[3]);
	strcat(executavel, ".e");

	char* modulo_b = (char*) calloc(1, sizeof(argv[2]));
	strcpy(modulo_b, argv[2]);
	strcat(modulo_b, ".o");

	char* modulo_a = (char*) calloc(1, sizeof(argv[1]));
	strcpy(modulo_a, argv[1]);
	strcat(modulo_a, ".o");

	int *novos_relativos = (int*) calloc(1, sizeof(int));
	int erro = 0;

	tabela_symbol_value tab_def[100];
	tabela_symbol_value tab_uso[100];
	codigo_objeto* codigo_concatenado;

	codigo_objeto* codigo_a = (codigo_objeto*) calloc(1, sizeof(codigo_objeto));
	contadores cont_a;
	inicializa_contador(&cont_a);

	FILE* fp_modulo_a;
	fp_modulo_a = fopen(modulo_a, "r");
	if(fp_modulo_a == NULL){
		printf("Arquivo %s não encontrado. \n", modulo_a);
		return 1;
	}
	codigo_a = criar_vetor_codigo(fp_modulo_a, codigo_a, &cont_a);
	int fator_de_correcao_a = 0;
	criar_tabela_def(fp_modulo_a, tab_def, &cont_a, fator_de_correcao_a);
	criar_tabela_uso(fp_modulo_a, tab_uso, &cont_a, fator_de_correcao_a);
	novos_relativos = criar_novos_relativos(fp_modulo_a, fator_de_correcao_a, &cont_a, novos_relativos);
	cont_a.indice_fator_correcao = cont_a.indice_relativo - 1;

	codigo_objeto* codigo_b = (codigo_objeto*) malloc(sizeof(codigo_objeto));
	contadores cont_b;
	inicializa_contador(&cont_b);
	FILE* fp_modulo_b;
	fp_modulo_b = fopen(modulo_b, "r");
	if(fp_modulo_b == NULL){
		printf("Arquivo %s nao encontrado. \n", modulo_b);
		return 1;
	}
	codigo_b = criar_vetor_codigo(fp_modulo_b, codigo_b, &cont_b);
	int fator_de_correcao_b = cont_a.indice_cod_obj - 1;

	codigo_concatenado = juntar_objetos(codigo_a, codigo_b, cont_a.indice_cod_obj, cont_b.indice_cod_obj);
	criar_tabela_def(fp_modulo_b, tab_def, &cont_a, fator_de_correcao_b);
	criar_tabela_uso(fp_modulo_b, tab_uso, &cont_a, fator_de_correcao_b);
	novos_relativos = criar_novos_relativos(fp_modulo_b, fator_de_correcao_b, &cont_a, novos_relativos);
	atualizar_tabela_relativos(novos_relativos, tab_uso, &cont_a);
	atualizar_objetos_tabela_relativos(novos_relativos, codigo_concatenado, &cont_a, fator_de_correcao_b);
	erro = atualizar_objeto_uso_def (codigo_concatenado, tab_uso, tab_def, &cont_a);

	if(!erro){
		int tamanho_total = cont_a.indice_cod_obj + cont_b.indice_cod_obj;
		gerar_executavel(codigo_concatenado, tamanho_total, executavel);
	}
	return 0;
}


/* Funcao que cria tabela de definicao geral - com definicoes de ambos arquivos, com valores atualizados com fator de correcao.
* Parametros 
*	FILE *fp - ponteiro do arquivo a ser analisado
*	tabela_symbol_value* tab_def - ponteiro de tabela de definicao global
*   contadores *cont - ponteiro de contadores, obter e atualizar valores de indice do vetor da tabela de definicao
*	int fator_de_correcao - fator de correcao a ser aplicado nos enderecos. 
*/
void criar_tabela_def(FILE* fp, tabela_symbol_value* tab_def,contadores* cont, int fator_de_correcao){
	rewind(fp);
	char token[63];
	int valor;

	do {
		fscanf(fp, "%s", token);
	} while(strcmp(token, "DEFINITION") != 0);

	fscanf(fp, "%s", token);
	while (strcmp(token, "RELATIVES") != 0){

		tab_def[(*cont).indice_tab_def].symbol = (char*) malloc(sizeof(token));
		strcpy(tab_def[(*cont).indice_tab_def].symbol, token);

		fscanf(fp, "%s", token);
		valor = atoi(token);	

		valor = valor + fator_de_correcao;

		tab_def[(*cont).indice_tab_def].value = valor;
		(*cont).indice_tab_def++;
		
		fscanf(fp, "%s", token);
	}
}

/* Funcao que cria tabela de uso geral - com definicoes de ambos arquivos, com valores atualizados com fator de correcao. 
* Parametros 
*	FILE *fp - ponteiro do arquivo a ser analisado
*	tabela_symbol_value* tab_uso - ponteiro de tabela de uso global
*   contadores *cont - ponteiro de contadores, obter e atualizar valores de indice do vetor da tabela de uso
*	int fator_de_correcao - fator de correcao a ser aplicado nos enderecos. 
*/
void criar_tabela_uso(FILE* fp, tabela_symbol_value* tab_uso,contadores* cont, int fator_de_correcao){
	rewind(fp);
	char token[63];
	int valor;

	do {
		fscanf(fp, "%s", token);
	} while(strcmp(token, "USE") != 0);

	fscanf(fp, "%s", token);
	while (strcmp(token, "TABLE") != 0){

		tab_uso[(*cont).indice_tab_uso].symbol = (char*) malloc(sizeof(token));
		strcpy(tab_uso[(*cont).indice_tab_uso].symbol, token);

		fscanf(fp, "%s", token);
		valor = atoi(token);	

		valor = valor + fator_de_correcao;

		tab_uso[(*cont).indice_tab_uso].value = valor;
		(*cont).indice_tab_uso++;
		
		fscanf(fp, "%s", token);
	}
}

/* Funcao que cria vetor com codigo objeto (sem nenhum tipo de tratamento) a partir do arquivo recebido como parametro.
*
* Parametros
*	FILE *fp - ponteiro do arquivo a ser analisado, onde sera obtido o codigo objeto.
*	codigo_objeto* codigo - ponteiro de codigo_objeto a ser atualizado.
*   contadores *cont - ponteiro de contadores, obter e atualizar valores de indice do codigo objeto.
*
* Retorno: codigo objeto sem nenhum tipo de tratamento.
*
*/
codigo_objeto* criar_vetor_codigo(FILE* fp, codigo_objeto* codigo, contadores* cont){
	
	char token[63];
	int valor;
	codigo_objeto* codigo_atualizado = codigo;

	do {
		fscanf(fp, "%s", token);
	} while(strcmp(token, "CODE") != 0);

	while (fscanf(fp, "%s", token) != EOF){
		codigo_atualizado = (codigo_objeto*) realloc(codigo_atualizado, (*cont).indice_cod_obj*(sizeof(codigo_objeto)));

		valor = atoi(token);

		codigo_atualizado[(*cont).indice_cod_obj-1].valor = valor;
		(*cont).indice_cod_obj++;
	}

	return codigo_atualizado;
}

/* Funcao que capta os valores "RELATIVES" do arquivo e adiciona num vetor. 
* 
* Parametros
*	FILE *fp - ponteiro do arquivo a ser analisado, onde serao obtidos os relativos.
*	int fator_de_correcao - fator de correcao a ser aplicado nos enderecos. 
*   contadores *cont - ponteiro de contadores, obter e atualizar valores de indice dos relativos.
* 	int* relativos - vetor dos relativos.
*
* Retorno: vetor dos relativos. 
*
*/
int* criar_novos_relativos(FILE* fp, int fator_de_correcao, contadores* cont, int* relativos){
	int valor_relativo;
	int* novos_relativos = relativos;
	char token[63];

	rewind(fp);

	do {
		fscanf(fp, "%s", token);
	} while(strcmp(token, "RELATIVES") != 0);

	fscanf(fp, "%s", token);
	while (strcmp(token, "CODE") != 0){

		novos_relativos  = (int*) realloc(novos_relativos, (*cont).indice_relativo * sizeof(int));
		valor_relativo = atoi(token);

		novos_relativos[(*cont).indice_relativo-1] = valor_relativo + fator_de_correcao;
		(*cont).indice_relativo++;

		fscanf(fp, "%s", token);
	}

	return novos_relativos;
}

/* Funcao que atualiza os relativos de maneira que os enderecos que estao simultaneamente na tabela de uso e nos relativos
* receberao o valor -1. Isso indicara que deverao ser ignorados. 
*
* Parametros
* 	int* relativos - vetor dos relativos que sera atualizado. 
* 	tabela_symbol_value* tab_uso - ponteiro da tabela de uso geral
*   contadores *cont - ponteiro de contadores, obter e atualizar valores de indice dos relativos.
*
*/
void atualizar_tabela_relativos(int* relativos, tabela_symbol_value* tab_uso, contadores* cont){
	int indice_relativo = (*cont).indice_relativo;
	int indice_tab_uso = (*cont).indice_tab_uso;
	int i = 0, j = 0;
	
	for (; i < indice_relativo - 1; i++){
		for(; j < indice_tab_uso; j++){
			if( relativos[i] == tab_uso[j].value){
				relativos[i] = -1;
				break;
			}
		}
		j = 0;
	}

}

/* Funcao que concatena os codigos objeto dos dois modulos. 
*
* Parametros
*	codigo_objeto* codigo_a - codigo objeto do primeiro modulo
*	codigo_objeto* codigo_b - codigo objeto do segundo modulo
*	int tamanho_a - tamanho do codigo objeto do primeiro modulo
*	int tamanho_b - tamanho do codigo objeto do segundo modulo
*
* Retorno: ponteiro de codigo_objeto que contem os dois codigos objeto concatenados. 
*	
*/
codigo_objeto* juntar_objetos(codigo_objeto* codigo_a, codigo_objeto* codigo_b, int tamanho_a, int tamanho_b){
	
	int tamanho_total = tamanho_a + tamanho_b - 2, i;

	codigo_objeto* novo_codigo_objeto = (codigo_objeto*) calloc(tamanho_total, sizeof(codigo_objeto));
	
	for(i = 0; i < tamanho_a; i++){
		novo_codigo_objeto[i] = codigo_a[i];
	}

	/* "tamanho_a --": ignorar o tamanho a mais acrescido ao final da iteracao no momento de criacao do codigo objeto*/ 
	for(i = 0, tamanho_a --; tamanho_a < tamanho_total; tamanho_a++){
		novo_codigo_objeto[tamanho_a] = codigo_b[i];
		i++;
	}

	return novo_codigo_objeto;
}


/* Funcao que atualiza o codigo objeto concatenado (concatenacao entre os dois modulos) de acordo com os relativos.
* A partir do vetor de relativos, atualiza-se, somando com fator de correcao, enderecos que nao possuem valor "-1". 
* A iteracao so comeca do momento em que eh necessario somar o fator de correcao, ou seja, nos locais onde as alteracoes
* seriam apenas "codigo[] += 0", o codigo ignora. O indice do inicio do segundo modulo na lista de relativos esta salvo 
* em "indice_fator_correcao", no contador. 
*
* Parametros
*	int* relativos - vetor de relativos. 
*	codigo_objeto* codigo - codigo objeto concatenado que sera atualizado
*	contadores* cont - ponteiro de contadores para obter indice do vetor de relativos.
*	int fator_de_correcao_b - fator de correcao do segundo modulo, considerando que o fator do primeiro eh zero. 
*
*/
void atualizar_objetos_tabela_relativos(int* relativos, codigo_objeto* codigo,contadores* cont, int fator_de_correcao_b){

	int i = (*cont).indice_fator_correcao;
	for(; i < (*cont).indice_relativo - 1; i++){
		if(relativos[i] != -1) {
			codigo[relativos[i]].valor +=  fator_de_correcao_b;
		}
	}
}

/* Funcao que procura um simbolos na tabela de definicao global. 
* 
* Parametros
*	char* symbol - simbolo a ser procurado na tabela de definicao global
*	tabela_symbol_value* tab_def - ponteiro da tabela de definicao global
* 	contadores* cont - ponteiro de contadores para obter informacoes do tamanho da tabela de definicao global. 
*
* Retorno: -1, se nao houver o simbolo na tabela, ou a posicao do simbolo na tabela. 
*
*/
int procura_tab_def(char* symbol, tabela_symbol_value* tab_def, contadores* cont) {

	int i;

	for(i = 0; i < (*cont).indice_tab_def; i++){
		if(strcmp(symbol,tab_def[i].symbol) == 0) {
			return i;
		}
	}

	return -1;
}


/* Funcao que atualiza o codigo objeto a partir das tabelas globais de uso e definicao, alterando nos locais indicados pela tabela
* de uso, adicionando ao valor original o valor indicado pela tabela de definicao, de acordo com o simbolo. 
*
* Parametros
*	codigo_objeto* codigo - ponteiro para o codigo objeto concatenado, que sera atualizado. 
*	tabela_symbol_value* tab_uso - ponteiro da tabela de uso global.
*	tabela_symbol_value* tab_def - ponteiro da tabela de definicao global. 
*	contadores* cont - ponteiro de contadores para obter o tamanho da tabela global de uso. 
*	
* Retorno: 0, caso nao existam erros, ou um valor maior que zero (o numero de errs), caso existam. 
*
*/
int atualizar_objeto_uso_def (codigo_objeto* codigo, tabela_symbol_value* tab_uso, tabela_symbol_value* tab_def, contadores* cont) {

	int i, posicao, erro = 0;

	for(i = 0; i < (*cont).indice_tab_uso; i++) {
		posicao = procura_tab_def(tab_uso[i].symbol, tab_def, cont);
		if(posicao >= 0){
			codigo[tab_uso[i].value].valor += tab_def[posicao].value;
		}
		else {
			printf("Erro de ligação: símbolo %s não encontrado na Tabela de Definição.\n", tab_uso[i].symbol);
			erro++;
		} 
	}

	return erro;

}


/* Funcao que imprime o codigo executavel em um arquivo.
*
* Parametros
*	codigo_objeto* codigo_concatenado - codigo objeto final
*	int tamanho_total - tamanho do codigo_concatenado
* 	char* executavel - nome do arquivo executavel que recebera o codigo objeto.
*/
void gerar_executavel(codigo_objeto* codigo_concatenado, int tamanho_total, char* executavel){
	
	int i;
	char* nome_arquivo = (char*) calloc(1, sizeof(executavel));
	nome_arquivo = executavel;

	FILE* arquivo_executavel;

	arquivo_executavel = fopen(nome_arquivo, "w");

	for(i = 0; i < tamanho_total - 2; i++){
		
		/*No vetor, os numeros no intervalo [0,10) nao estao com o zero "a mais" a esquerda. Estao sendo adicionados abaixo*/
		if( (codigo_concatenado[i].valor >= 0) && (codigo_concatenado[i].valor < 10)){
			fprintf(arquivo_executavel, "0");
		}
		fprintf(arquivo_executavel, "%d ", codigo_concatenado[i].valor);
	}
	printf("\n");
}

/* Funcao que inicializa o contador. 
*
* Parametros
* 	contadores* cont - ponteiro para contadores a ser inicializado. 
*
* OBS: os indices tambem podem ser interpretados como o tamanho. 
*/
void inicializa_contador(contadores* cont){
	(*cont).indice_cod_obj = 1;
	(*cont).indice_tab_def = 0; 
	(*cont).indice_tab_uso = 0; 
	(*cont).indice_relativo = 1; 
	(*cont).indice_fator_correcao = 0;
}
