#include <stdio.h>
#include <algorithm>
#include <cassert>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <vector>
#include <iterator>
#include <sstream>
#include <iostream>
#include <boost/range/algorithm_ext/erase.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;

void identificarComando(string &comando);

/**
 *	Seraração de Strings em vetores
 * 
 * */
void split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

/**
 *	Remoção de espaços consecutivos
 * */
bool BothAreSpaces(char lhs, char rhs) { 
	return ((lhs == rhs) && (lhs == ' ' || lhs == '\t')) 
			|| (lhs == '\t' && rhs == ' ')
			|| (lhs == ' ' && rhs == '\t'); 
}

void limpar(string &s){
	string::iterator novo_fim = unique(s.begin(), s.end(), BothAreSpaces);
	s.erase(novo_fim, s.end());   
}

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
	boost::erase_all(comando, "\n");
	std::replace( comando.begin(), comando.end(), '\t', ' ');
	
	identificarComando(comando);
	free(linha);
}

/**
 *	Função de parsing do comando passado.
 *
 *	PRE: comando é válido, string != NULL
 * */
void identificarComando(string &comando){
	
	if(comando.empty()){
		return;
	}
	
	limpar(comando);

	//Lista de cláusulas
	vector<string> clausulas(split(comando, ' '));
	
	//Estado inicial: lendo SELECT
	int estado = 0;

	vector<string> atributos;
	string relacao, where, relacaoB, condicao;

	for (int i = 0; i < clausulas.size(); i++){
		switch(estado){
			case 0:
				//Comando SELECT, continua
				estado++;
				continue;
				break;
			case 1:
				//ler lista de atributos
				atributos = split(clausulas[i], ',');
				estado++;
				break;
			case 2:
				//Clausula FROM
				if(clausulas.size() < 7){
					//Caso de uma seleção sem join, possivelmente com where
					estado++;
				} else {
					// caso de uma seleção com join, possivelmente com where
					estado += 3;				
				}
				break;
			case 3:
				//seleção -join -where
				//nome da relação
				relacao = clausulas[i];
				
				//seleção -join +where
				if(clausulas.size() > 4){
					estado++;
				} else{
					//fim
					boost::erase_all(relacao, ";");
					estado = 99;
				}
				break;
			case 4:
				//condição where
				if(clausulas[i] == "WHERE") continue;

				//seleção -join +where
				where = clausulas[i];
				estado = 99;
				break;
			case 5:
				//seleção +join ?where
				if(clausulas[i] == "JOIN") {
					estado++;
					break;
				}

				relacao = clausulas[i];
				boost::erase_all(relacao, "(");
				
				break;
			case 6:
				//seleção +join
				if(clausulas[i] == "ON"){
					estado++;
					break;
				}

				relacaoB = clausulas[i];
				break;
			case 7:
				condicao = clausulas[i];
				boost::remove_erase_if(condicao, boost::is_any_of(");"));

				if(clausulas.size() > 8){
					estado++;
					continue;
				}
				
				estado = 99;
				break;
			case 8:
				if(clausulas[i] == "WHERE") continue;

				where = clausulas[i];
				boost::erase_all(where, ";");
				estado = 99;
				break;
			default:
				break;
		}
	}

}

/*int main(int argc, char **argv){
	FILE *arq = fopen(argv[1], "r");
	select(arq);
	select(arq);
	select(arq);
	select(arq);
	select(arq);
	fclose(arq);
}*/
