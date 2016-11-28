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
	
	assert(comando.empty() && "Comando inválido.\n");

	stringstream fluxo(comando);
	istream_iterator<string> comeco(fluxo);
	istream_iterator<string> fim;
	vector<string> tokens(comeco, fim);

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
