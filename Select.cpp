#include <stdio.h>
#include <algorithm>
#include <cassert>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <sstream>
#include <iostream>
#include <boost/range/algorithm_ext/erase.hpp>
#include <boost/algorithm/string.hpp>
#include <errno.h>
#include <cassert>

/**
 *	Util
 * */

using namespace boost;
using namespace std;

void identificarComando(string &comando);

/**
 *	Seraração de Strings em vetores
 * 
 * */
void split(const string &s, char delim, vector<string> &elems) {
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
	string::iterator novo_fim = unique(s.begin(), s.end(), BothAreSpaces);
	s.erase(novo_fim, s.end());   
}

/**
 *
 *	Álgebra Relacional
 *
 * */

string escreverProjecao(string relacao, int qtdAtributos, string listaAtributos, string nomeProjecao){
	fstream arquivo;
	arquivo.open("Operacao.alg", fstream::app);

	arquivo << "P(" << relacao << "," << qtdAtributos << "," << listaAtributos << "," << nomeProjecao << ")\n";

	arquivo.close();

	return nomeProjecao;
}

string escreverSelecao(string relacao, string atr, string op, string valor, string nomeSelecao){
	fstream arquivo;
	arquivo.open("Operacao.alg", fstream::app);
	arquivo << "S(" << relacao << "," << atr << "," << op << "," << valor << ", " << nomeSelecao << ")\n";
	arquivo.close();

	return nomeSelecao;
}

string escreverJuncao(string relacao, string relacaoB, string condicao, string nomeJuncao){
	fstream arquivo;
	arquivo.open("Operacao.alg", fstream::app);

	arquivo << "J(" << relacao << "," << relacaoB << "," << condicao << "," << nomeJuncao << ")\n";

	return nomeJuncao;
}

/**	
 *	Parser Álgebra Relacional
 *
 *	TODO: relacionar com catalogo e dados
 * */

vector<string> separarCampos(string linha){
	size_t inicio = 2;
	size_t fim = linha.length() - 2;

	string sublinha = linha.substr(inicio, fim);

	return split(sublinha, ',');
}

void executarProjecao(string linha){

	vector<string> dados = separarCampos(linha);

	string 	nomeTabela = dados[0],
			nomeCatalogo = nomeTabela + ".ctl",
			nomeDados = nomeTabela + ".dad";

	const char *qtd_c = dados[1].c_str();

	int tam = atoi(qtd_c);

	vector<string> atrs;
	
	for(int i = 2; i < 2 + tam; i++){
		atrs.push_back(dados[i]);
	}

	string nomeResultante(dados[2 + tam]);
	
	// TODO: escrever execução em arquivo e na tela
	// 1. Criar arquivo de catálogo final
	fstream saidaCtl;
	saidaCtl.open(nomeResultante.c_str(), fstream::out);

	// 1. Ler arquivo de catálogo
	fstream catalogo;
	catalogo.open(nomeCatalogo.c_str(), fstream::in);

	int grau, cardinalidade,
		grauProj = 0, cardProj = 0;
	string lin,
		   saidaCtlStr;

	catalogo >> lin;
	
	// TODO: testar
	assert(2 == scanf(lin.c_str(), "%d,%d\n", &grau, &cardinalidade) 
			&& "Erro na leitura da cardinalidade e grau.\n");
	
	//Construção do CTL
	string atributo;
	vector<string> modificadores;

	for(int i = 0; i < grau; i++){
		catalogo >> atributo;
		modificadores = split(atributo, ',');
		if(find(atrs.begin(), atrs.end(), modificadores[0]) != atrs.end()){
			grauProj++;
			saidaCtlStr += atributo + "\n";
		}
	}

	// Construção do DAD

	/*
	cout << "Grau:" << grau << "\n";
	cout << "Cardinalidade:" << cardinalidade << "\n";
	*/

	// 2. Ler arquivo de dados
	fstream arqDados;
	arqDados.open(nomeDados.c_str(), fstream::in);
	// 3. Executar projeção
	// 4. Escrever em arquivo
	// 5. Escrever na tela
	catalogo.close();
	arqDados.close();
	saidaCtl.close();
}

void executarSelecao(string linha){

	vector<string> dados = separarCampos(linha);

	string nomeTabela(dados[0]),
		   nomeAtr(dados[1]),
		   operador(dados[2]),
		   valor(dados[3]),
		   nomeFinal(dados[4]);

	// TODO: escrever execucao em arquivo e na tela
}

void executarJuncao(string linha){

	vector<string> dados = separarCampos(linha);

	string nomeTabA(dados[0]),
		   nomeTabB(dados[1]),
		   cond(dados[2]),
		   nomef(dados[3]);

	// TODO: escrever execucao em arquivo e na tela
}

void parse(){
	fstream arquivo;
	arquivo.open("Operacao.alg", fstream::in);

	string linha;
	
	while(arquivo >> linha){
		switch(linha[0]){
			case 'P':
				executarProjecao(linha);
				break;
			case 'S':
				executarSelecao(linha);
				break;
			case 'J':
				executarJuncao(linha);
				break;
			default:
				break;
		}
	}

	arquivo.close();
}

/**
 *	SQL
 * */

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
	erase_all(comando, "\n");
	replace( comando.begin(), comando.end(), '\t', ' ');
	
	identificarComando(comando);
	free(linha);
}

string parseAtr(string where){
	size_t found = where.find_first_of("=<>");	

	return where.substr(0, found);
}

string parseOp(string where){
	size_t last = where.find_last_of("=<>"),
		   first = where.find_first_of("=<>");

	string t(1, where.at(last));

	return ( first == last ) ?  t : where.substr(first, last - first);
}

string parseVal(string where){
	size_t found = where.find_last_of("=<>");	

	return where.substr(found + 1, where.length());
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

	string atributos;
	string relacao, where, relacaoB, condicao, 
		   selecao, projecao, juncao;

	for (int i = 0; i < clausulas.size(); i++){
		switch(estado){
			case 0:
				//Comando SELECT, continua
				estado++;
				continue;
				break;
			case 1:
				//ler lista de atributos
				//atributos = split(clausulas[i], ',');
				atributos = clausulas[i];

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
					//fim: projeção na relacao, com os atributos
					erase_all(relacao, ";");

					escreverProjecao(relacao, count(atributos.begin(), atributos.end(), ',') + 1 , 
							atributos, relacao + "_P_" + atributos);

					estado = 99;
				}

				break;
			case 4:
				//condição where
				if(clausulas[i] == "WHERE") continue;

				//seleção -join +where
				where = clausulas[i];
				erase_all(where, ";");
				
				selecao = escreverSelecao(relacao, parseAtr(where), parseOp(where), 
						parseVal(where), relacao + "_S_" + where);
				escreverProjecao(selecao, count(atributos.begin(), atributos.end(), ',') + 1,
						atributos, selecao + "_P_" + atributos);

				estado = 99;
				break;
			case 5:
				//seleção +join ?where
				if(clausulas[i] == "JOIN") {
					estado++;
					break;
				}
				relacao = clausulas[i];
				erase_all(relacao, "(");
				
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
				remove_erase_if(condicao, is_any_of(");"));

				if(clausulas.size() > 8){
					estado++;
					continue;
				}

				juncao = escreverJuncao(relacao, relacaoB, condicao, 
						relacao + "_J_" + condicao + "_" + relacaoB);
				escreverProjecao(juncao, count(atributos.begin(), atributos.end(), ',') + 1,
						atributos, juncao + "_P_" + atributos);

				estado = 99;
				break;
			case 8:
				if(clausulas[i] == "WHERE") continue;

				where = clausulas[i];
				erase_all(where, ";");

				juncao = escreverJuncao(relacao, relacaoB, condicao,
						relacao + "_J_" + condicao + "_" + relacaoB);
				selecao = escreverSelecao(juncao, parseAtr(where), parseOp(where), 
						parseVal(where), juncao + "_S_" + where);
				escreverProjecao(selecao, count(atributos.begin(), atributos.end(), ',') + 1, 
						atributos, selecao + "_P_" + atributos);

				estado = 99;
				break;
			default:
				break;
		}
	}

}

int main(int argc, char **argv){
	FILE *arq = fopen(argv[1], "r");
	select(arq);
	select(arq);
	select(arq);
	select(arq);
	select(arq);

	parse();
	fclose(arq);
}
