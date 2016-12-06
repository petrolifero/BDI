#include <stdio.h>
#include <map>
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

vector<int> split(const string &s, string delim){
	vector<int> elems;
	vector<string> aux = split(s, delim.at(0));
	for(int i = 0; i < aux.size(); i++){
		elems.push_back(atoi(aux[i].c_str()));
	}
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

vector<string> separarParametros(string linha){
	size_t inicio = 2;
	size_t fim = linha.length() - 2;

	string sublinha = linha.substr(inicio, fim);

	return split(sublinha, ',');
}

void executarProjecao(string linha){

	// Parâmetros da operação algébrica de projeção
	vector<string> parametros = separarParametros(linha);

	// nomes de parâmetros em particular
	string 	nomeTabela = parametros[0],
			nomeCatalogo = nomeTabela + ".ctl",
			nomeDados = nomeTabela + ".dad";

	int tam = atoi(parametros[1].c_str()), // qtd de atrs na projeção
		inGrau, //grau da relação de entrada
		inCard, //cardinalidade da relação de entrada
		projGrau = 0; // grau da projeção
	
	//atributo da entrada sendo lido no Ctl
	string inAtr;

	//Atributos selecionados na projeção
	vector<string> projAtrs;
	
	//nomes dos arquivos
	string nomeResultante(parametros[2 + tam]),
		   nomeResCtl(nomeResultante + ".ctl"),
		   nomeResDad(nomeResultante + ".dad");

	//Arquivos utilizados
	fstream projCtl, //Ctl da projeção
			projDad, //dad da projeção
			inCtl, 	//ctl da entrada
			inDad; //dad da entrada

	for(int i = 2; i < 2 + tam; i++){
		projAtrs.push_back(parametros[i]);
	}
	
	// 1. Ler arquivo de catálogo
	inCtl.open(nomeCatalogo.c_str(), fstream::in);

	// 2. Criar arquivo de catálogo final
	projCtl.open(nomeResCtl.c_str(), fstream::out);

	inCtl >> inAtr;
	
	// TODO: testar
	assert(2 == scanf(inAtr.c_str(), "%d,%d\n", &inGrau, &inCard) 
			&& "Erro na leitura da cardinalidade e grau.\n");
	
	//Construção do CTL
	map<int, string> projCtlCols;

	// coluna e seus modificadores 
	vector<string> inMods;

	for(int i = 0; i < inGrau; i++){
		inCtl >> inAtr;
		inMods = split(inAtr, ',');
		//se a coluna está na projeção...
		if(find(projAtrs.begin(), projAtrs.end(), inMods[0]) != projAtrs.end()){
			// acrescente-a ao mapa
			projCtlCols.insert(make_pair(i, inAtr));
			//aumente o grau da projeção
			projGrau++;
		}
	}

	projCtl << projGrau << "," << inCard << endl;
	cout << projGrau << "," << inCard << endl;

	std::map<int, string>::iterator it = projCtlCols.begin();
    while(it != projCtlCols.end())
    {
        projCtl << it->second << endl;
		cout << it->second << endl;
        it++;
    }

	// 3. Escrever arquivo Dad da projeção
	projDad.open(nomeResDad.c_str(), fstream::out);
	// Iterar sobre os índices no mapa, só obtendo do DAD da relação aqueles valores dos índices passados
	inDad.open(nomeDados.c_str(), fstream::in);

	//linha com os valores da i-ésima tupla
	string inVal;
	vector<int> inVals;
	
	for(int i = 0; i < inCard; i++){
		inDad >> inVal;
		inVals = split(inVal, " ");

		std::map<int, string>::iterator itDad = projCtlCols.begin();
		while(it != projCtlCols.end())
		{
			projDad << inVals[it->first] << " ";
			cout << inVals[it->first] << " ";
			it++;
		}
		projDad << endl;
		cout << endl;
	}

	//Escrita do arquivo

	inCtl.close();
	inDad.close();
	projCtl.close();
	projDad.close();
}

void executarSelecao(string linha){

	vector<string> parametros = separarParametros(linha);

	string nomeTabela(parametros[0]),
		   nomeCatalogo = nomeTabela + ".ctl",
		   nomeDados = nomeTabela + ".dad",

		   nomeAtr(parametros[1]),
		   operador(parametros[2]),
		   valor(parametros[3]),

		   nomeFinal(parametros[4]),
		   selNomeCtl(nomeFinal + ".ctl"),
		   selNomeDad(nomeFinal + ".dad");

	fstream inCtl,
			inDad,
			selCtl,
			selDad;

	int inGrau,
		inCard,
		selCard;

	string inAtr;


	inCtl.open(nomeCatalogo.c_str(), fstream::in);
	inDad.open(nomeDados.c_str(), fstream::in);

	inCtl >> inAtr;

	assert(2 == scanf(inAtr.c_str(), "%d,%d\n", &inGrau, &inCard) 
			&& "Erro na leitura da cardinalidade e grau.\n");

	selCtl.open(selNomeCtl.c_str(), fstream::out);
	selDad.open(selNomeDad.c_str(), fstream::out);

	// TODO: encontrar atributo sendo selecionado
	// TODO: aplicar teste à cada tupla
	

	selCtl << inGrau << "," << selCard << endl;

	//ctl da relação é igual à da seleção
	while(inCtl >> inAtr){
		selCtl << inAtr;
	}

	inCtl.close();
	inDad.close();
	selCtl.close();
	selDad.close();
}

void executarJuncao(string linha){

	vector<string> dados = separarParametros(linha);

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
