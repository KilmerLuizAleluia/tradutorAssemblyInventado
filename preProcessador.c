#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
* 1 struct nova - map. As outras duas serao unidas com as suas!
*/
typedef struct map map;
typedef struct contadores contadores;
typedef struct flags flags;


/*
*   Assinatura das funcoes utilizadas no pre-processamento.
*/
void pre_processador(FILE *fp, char* token, char* codigo_apos_pre_processamento, contadores* cont, flags* flgs);
void retira_comentario(FILE *fp, char* codigo_apos_pre_processamento, contadores* cont);
void ignorar_proxima_linha(FILE* fp, contadores* cont, char* codigo_apos_pre_processamento);
void tratamento_equ(FILE *fp, char* codigo_apos_pre_processamento, char* token, map* mapa, int indice_mapa, char* auxiliar_rotulo, contadores* cont, flags* flgs);
void tratamento_if(FILE *fp, char* token, int indice_mapa, map* mapa, char* codigo_apos_pre_processamento, contadores *cont, flags* flgs);
int verifica_substituicao_equ(int indice_mapa, char* token, map* mapa, char* codigo_apos_pre_processamento, contadores* cont);
void transformaMaiusculo (char* token, int comprimento);
int conferir_quebra_linha(FILE* fp, char* codigo_apos_pre_processamento, contadores* cont);
void inicializa_contador(contadores *cont);
void inicializa_flags(flags *flgs);
void conferir_espaco_branco(FILE* fp, char* codigo_apos_pre_processamento, contadores* cont);


/*
* Estrutura nova. É uma estrutura de mapa, onde para cara chave, há um valor. 
*/
struct map{
    char* key;
    char* value;
};

struct contadores{
    int contador_linha;
    int eof;
};

struct flags {
    int erro;
};

/* Funcao principal. Aloca espacos para variaveis iniciais essenciais. Inicializa contadores e flags. Chama o pre-processador.
*
* Opcao no comentario "debug": imprime conteudo de "codigo_apos_pre_processamento", numero de linhas contabilizadas e numero de erros. 
*
*/
int main() {

    FILE *fp;
    char token[255];
    char* codigo_apos_pre_processamento;
    int tamanho_para_alocar;
    contadores cont;
    flags flgs;
    
    inicializa_contador(&cont);
    inicializa_flags(&flgs);

    fp = fopen("teste0.txt", "r");
    /* Tamanho máximo do codigo_apos_pre_processamento.*/
    fseek(fp, 0L, SEEK_END);
    tamanho_para_alocar = ftell(fp);
    rewind(fp);
    codigo_apos_pre_processamento = (char*) malloc(tamanho_para_alocar*sizeof(char));
    
    if (fp) {
        pre_processador(fp, token, codigo_apos_pre_processamento, &cont, &flgs);
        fclose(fp);
    }

    /*** DEBUG *** *** DEBUG *** *** DEBUG ***/
    
    printf("\n---- **** ---- **** ---- **** ---- **** \n");

    printf("Inicio abaixo:\n%s\n", codigo_apos_pre_processamento);

    printf("\n---- **** ---- **** ---- **** ---- **** \n");

    printf("cont.contador_linha: %d\n", cont.contador_linha);

    printf("\nflags: %d\n", flgs.erro);

    free(codigo_apos_pre_processamento);
    return 0;
}

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
        * printf("TOKEN: %s --- LINHA: %d\n", token, (*cont).contador_linha);
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

    while( ((caracter = fgetc(fp)) != EOF) &&  caracter == '\n') {
        codigo_apos_pre_processamento[size] = '\n';
        codigo_apos_pre_processamento[size + 1] = '\0';
        (*cont).contador_linha++;
        linha_quebrada = 1;

        /* Se houve quebra de linha, o tamanho da string codigo_apos_pre_processamento eh atualizado*/
        size = strlen(codigo_apos_pre_processamento);
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
    (*cont).contador_linha++;
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
    while((caracter = fgetc(fp)) != '\n');
    
    codigo_apos_pre_processamento[size] = '\n';
    codigo_apos_pre_processamento[size + 1] = '\n';
    codigo_apos_pre_processamento[size + 2] = '\0';
    (*cont).contador_linha++;
    (*cont).contador_linha++;
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
        printf("Erro sintático, linha %d. Operador EQU com número inválido de operandos. Apenas o primeiro operando será considerado.\n", (*cont).contador_linha);
        while( ((ignorador = fgetc(fp)) != EOF) &&  ignorador != '\n');

        /* No if abaixo, significa que o arquivo terminou*/
        if(ignorador == -1){
            (*cont).eof = 1;
        } 
        /* No else abaixo, significa que o char eh um '\n'.*/
        else{
            (*cont).contador_linha++;
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
        printf("Erro sintático, linha %d. Operador IF com número inválido de operandos. Apenas o primeiro operando será considerado.\n", (*cont).contador_linha);
        while( ((ignorador = fgetc(fp)) != EOF) &&  ignorador != '\n');

        /* No if abaixo, significa que o arquivo terminou*/
        if(ignorador == -1){
            (*cont).eof = 1;
        } 
        /* No else abaixo, significa que o char eh um '\n'.*/
        else{
            (*cont).contador_linha++;
        }

        (*flgs).erro++;
    }

    /* Nesse caso a diretiva IF estava falsa*/
    if(!encontrado){
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

/* Funcao que transforma um token qualquer em um token com letras maiusculas.
*
* Parametros
*   char* token - token a ser transformado uma palavra com apenas letras maiusculas
*   int comprimento - tamanho do token
*
*/
void transformaMaiusculo (char* token, int comprimento) { /*TESTADO*/
    int i; /*variável auxiliar do loop*/

    for(i=0;i<comprimento;i++) {                /*para cada elemento da string vemos se ele está em letra minúscula*/
        if(token[i]>=97 && token[i]<=122) { /*ou seja, possui o ascii entre 97 = 'a' e 122 = 'z'*/
            token[i] = token[i] - 32;   /*Se tivermos um desses casos, subtraímos 32 para obter a versão maiúscula da letra*/
        }                   /*Os outros ascii não precisam ser incluídos pois a questão maiúscula-minúscula*/
    }                       /*só envolve LETRAS*/
}


/* Funcao que inicializa os contadores.
*
* Parametros
*   contadores* cont - ponteiro de contadores que sera inicializado.
*
*/
void inicializa_contador(contadores* cont){
    (*cont).contador_linha = 1;
    (*cont).eof = 0;
}

/* Funcao que inicializa as flags. 
* 
* Parametros
*   flags* flgs - ponteiro de flags que sera inicializada.
*
*/
void inicializa_flags(flags* flgs){
    (*flgs).erro = 0;
}
