#include <string>
#include <algorithm>
#include <fstream>
#include "util.cpp"

using namespace std;

string parseAtr(string where){
	size_t found = where.find_first_of("=<>");	

	return where.substr(0, found);
}

string parseVal(string where){
	size_t found = where.find_last_of("=<>");	

	return where.substr(found + 1, where.length());
}

/**
 *	PRE: primeiro atributo pertence à primeira tabela
 * */
string parseFirstAtrInTab(string where){
	size_t found = where.find_first_of(".");

	return parseAtr((found != string::npos) ? where.substr(found) : where);
}

/**
 *	PRE: ultimo atributo pertence à 2ª tabela
 * */
string parseLastAtrInTab(string where){
	size_t found = where.find_last_of(".");

	return (found != string::npos) ? where.substr(found) : parseVal(where);
}

string parseOp(string where){
	size_t last = where.find_last_of("=<>"),
		   first = where.find_first_of("=<>");

	string t(1, where.at(last));

	return ( first == last ) ?  t : where.substr(first, last - first);
}

/**
 *	TODO Proposta: caminho como entrada da função
 *
 *	@return nome da projeção gerada, ou "" caso a relação de entrada seja inválida
 * */
string escreverProjecao(string relacao, int qtdAtributos, string listaAtributos, string nomeProjecao){
	if(relacao.empty()) return "";

	fstream arquivo;
	abrir(arquivo, "temp/Operacao.alg", fstream::app);

	replace(nomeProjecao.begin(), nomeProjecao.end(), ',', '-');
	arquivo << "P(" << relacao << "," << qtdAtributos << "," << listaAtributos << "," << nomeProjecao << ")\n";

	arquivo.close();

	return nomeProjecao;
}

/**
 *	TODO Proposta: caminho como entrada da função
 * */
string escreverSelecao(string relacao, string atr, string op, string valor, string nomeSelecao){
	fstream arquivo;
	abrir(arquivo, "temp/Operacao.alg", fstream::app);
	arquivo << "S(" << relacao << "," << atr << "," << op << "," << valor << "," << nomeSelecao << ")\n";
	arquivo.close();

	return nomeSelecao;
}

/**
 *	TODO Proposta: caminho como entrada da função
 * */
string escreverJuncao(string relacao, string relacaoB, string condicao, string nomeJuncao){
	fstream arquivo;
	abrir(arquivo, "temp/Operacao.alg", fstream::app);

	arquivo << "J(" << relacao << "," << relacaoB << "," << condicao << "," << nomeJuncao << ")\n";

	return nomeJuncao;
}
