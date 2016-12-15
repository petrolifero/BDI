#include <fstream>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <vector>
#include <boost/range/algorithm_ext/erase.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;

/**
 *	Abertura responsiva de arquivos. Em erro, @msg é escrita em stdout, com o programa terminando
 * */
void abrir(fstream &arquivo, const char* caminho, ios_base::openmode mode = fstream::in, 
		string msg = "Erro abrindo o arquivo: "){

	arquivo.open(caminho, mode);
	if ( (arquivo.rdstate() & std::ifstream::failbit ) != 0 ){
		std::cerr << msg << caminho << endl;
		exit(1);
	}
}

/**
 *	Separação de Strings em vetores
 */
void split(const string &s, char delim, vector<string> &elems) {
	if(s.empty()) return;

    stringstream ss;
    ss.str(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

vector<string> split(const string &s, char delim) {
    vector<string> elems;
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
	if(s.empty()) return;

	string::iterator novo_fim = unique(s.begin(), s.end(), BothAreSpaces);
	s.erase(novo_fim, s.end());   
}


/**
 *	Separa strings delimitadas por vírgulas
 * */
vector<string> separarParametros(string linha){
	if(linha.empty()){
		vector<string> vec;
		return vec;
	}

	size_t inicio = 2;
	size_t fim = linha.length() - 2;

	string sublinha = linha.substr(inicio, fim);

	return split(sublinha, ',');
}
