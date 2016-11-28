#include <stdio.h>
#include <algorithm>
#include <cassert>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iterator>
#include <sstream>
#include <iostream>

using namespace std;

void identificarComando(string comando);

// Função de interface com código em C
// obtém comando do arquivo, e passa para o manipulador do select
// PRE: arquivo está posicionado num SELECT
void select(FILE *ponteiroArquivo){
	assert(ponteiroArquivo != NULL && "Ponteiro de arquivo inválido");
	
	int rc = 0;
	char *linha = NULL;
	size_t tam = 0;
	
	rc = getdelim(&linha, &tam, ';', ponteiroArquivo);
	assert(rc != -1 && "Arquivo corrompido: falha na leitura de um SELECT\n");

	string comando(linha);
	replace( comando.begin(), comando.end(), '\n', ' ');
	
	identificarComando(comando);
	free(linha);
}

/**
 *	Função de parsing do comando passado.
 *
 *	PRE: comando é válido, string != NULL
 * */
void identificarComando(string comando){
	
	if(comando.empty()){
		return;
	}

	//Lista de cláusulas
	stringstream fluxoComando(comando);
	istream_iterator<string> comeco(fluxoComando);
	istream_iterator<string> fim;
	vector<string> clausulas(comeco, fim);

	// Lista de atributos
	stringstream fluxoAtributos(clausulas[1]);
	istream_iterator<string> comecoAtr(fluxoAtributos);
	istream_iterator<string> fimAtr;
	vector<string> atributos(comecoAtr, fimAtr);

	//Junção
	if(clausulas[4] == "JOIN"){
		string relacaoA, relacaoB;
		string condicao;
		
		relacaoA = clausulas[3];
		relacaoB = clausulas[5];
		condicao = clausulas[7];
	} else {
		string relacao = clausulas[3];
	}

	/** codificação: 
	 *	1º: Comando SELECT
	 *  2º: lista de atributos, separada por ','
	 *  3º: Cláusula FROM
	 *  próx(autômato) : relação OU junção de relações, (rel1 JOIN rel2 ON cond)
	 *  Cláusula WHERE (opcional)
	 */

}


int main(int argc, char **argv){
	FILE *arq = fopen(argv[1], "r");
	select(arq);
	fclose(arq);
}
