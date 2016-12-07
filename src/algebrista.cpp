#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <boost/range/algorithm_ext/erase.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <iostream>

using namespace std;
using namespace boost;

/**
 *	Separação de Strings em vetores
 */
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
 *	Separa strings delimitadas por vírgulas
 * */
vector<string> separarParametros(string linha){
	size_t inicio = 2;
	size_t fim = linha.length() - 2;

	string sublinha = linha.substr(inicio, fim);

	return split(sublinha, ',');
}

/**
 *
 *	Álgebra Relacional
 *
 * */
using namespace std;

/**
 *	TODO Proposta: caminho como entrada da função
 * */
string escreverProjecao(string relacao, int qtdAtributos, string listaAtributos, string nomeProjecao){
	ofstream arquivo;
	arquivo.open("etc/Operacao.alg", fstream::app);

	arquivo << "P(" << relacao << "," << qtdAtributos << "," << listaAtributos << "," << nomeProjecao << ")\n";

	arquivo.close();

	return nomeProjecao;
}

/**
 *	TODO Proposta: caminho como entrada da função
 * */
string escreverSelecao(string relacao, string atr, string op, string valor, string nomeSelecao){
	ofstream arquivo;
	arquivo.open("etc/Operacao.alg", ofstream::app);
	arquivo << "S(" << relacao << "," << atr << "," << op << "," << valor << ", " << nomeSelecao << ")\n";
	arquivo.close();

	return nomeSelecao;
}

/**
 *	TODO Proposta: caminho como entrada da função
 * */
string escreverJuncao(string relacao, string relacaoB, string condicao, string nomeJuncao){
	ofstream arquivo;
	arquivo.open("etc/Operacao.alg", ofstream::app);

	arquivo << "J(" << relacao << "," << relacaoB << "," << condicao << "," << nomeJuncao << ")\n";

	return nomeJuncao;
}

/**	
 *	Parser Álgebra Relacional
 *
 * */

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

	ifstream inCtl,//ctl da entrada
			 inDad;//dad da entrada

	ofstream projCtl,//Ctl da projeção
			 projDad;//dad da projeção

	for(int i = 2; i < 2 + tam; i++){
		projAtrs.push_back(parametros[i]);
	}
	
	// 1. Ler arquivo de catálogo
	inCtl.open(nomeCatalogo.c_str());

	// 2. Criar arquivo de catálogo final
	projCtl.open(nomeResCtl.c_str());

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
	projDad.open(nomeResDad.c_str());
	// Iterar sobre os índices no mapa, só obtendo do DAD da relação aqueles valores dos índices passados
	inDad.open(nomeDados.c_str());

	//linha com os valores da i-ésima tupla
	string inVal;
	vector<string> inVals;
	
	for(int i = 0; i < inCard; i++){
		inDad >> inVal;
		inVals = split(inVal, ' ');

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

bool _satisfaz(string valAtr, string tipoAtr, string operador, string val){
	if(tipoAtr == "I"){
		int valorAtr, valor, rc;
		rc = scanf(valAtr.c_str(), "%d", &valorAtr);
		assert(rc == 1 && "erro na leitura do valor na relação.\n");
		rc = scanf(val.c_str(), "%d", &valor);
		assert(rc == 1 && "erro na leitura do valor de seleção.\n");

		if(operador == "="){
			return valorAtr == valor;
		} else if(operador == ">"){
			return valorAtr > valor;
		} else if(operador == "<"){
			return valorAtr < valor;
		} else if(operador == ">="){
			return valorAtr >= valor;
		} else if(operador == "<="){
			return valorAtr <= valor;
		} else if(operador == "<>"){
			return valorAtr != valor;
		}

	} else if(tipoAtr == "C" ){
		if(operador == "="){
			return valAtr == val;
		} else if(operador == ">"){
			return valAtr > val;
		} else if(operador == "<"){
			return valAtr < val;
		} else if(operador == ">="){
			return valAtr >= val;
		} else if(operador == "<="){
			return valAtr <= val;
		} else if(operador == "<>"){
			return valAtr != val;
		}
	}

	assert(false && "Tipo inválido, não é I, e nem C.\n");
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

	ifstream inCtl,
			inDad;

	ofstream selCtl,
			selDad;

	int inGrau,
		inCard,
		selCard,
		selInd; //índice do atributo sendo selecionado

	string inAtr,
		   inVal,
		   selTipo;

	inCtl.open(nomeCatalogo.c_str(), fstream::in);
	inDad.open(nomeDados.c_str(), fstream::in);

	inCtl >> inAtr;

	assert(2 == scanf(inAtr.c_str(), "%d,%d\n", &inGrau, &inCard) 
			&& "Erro na leitura da cardinalidade e grau.\n");

	selCtl.open(selNomeCtl.c_str());
	selDad.open(selNomeDad.c_str());

	vector<string> inMods;

	for(int i = 0; i < inGrau; i++){
		inCtl >> inAtr;
		inMods = split(inAtr, ',');

		if(inMods[0] == nomeAtr){
			selInd = i;
			selTipo = inMods[1];
		}
	}

	// vetor com a i-ésima tupla da relação
	vector<string> inVals;
	for(int i = 0; i < inCard; i++){
		inDad >> inVal;
		inVals = split(inVal, ' ');

		if(_satisfaz(inVals[selInd], selTipo, operador, valor)){
			std::vector<string>::iterator itDad = inVals.begin();
			while(itDad != inVals.end())
			{
				selDad << *itDad << " ";
				cout << *itDad << " ";
				itDad++;
			}
		}
		selDad << endl;
		cout << endl;
	}

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

void abrir(fstream arquivo, const char* caminho, ios_base::openmode mode = fstream::in, 
		string msg = "Erro abrindo o arquivo: "){
	arquivo.open(caminho, mode);
	if ( (arquivo.rdstate() & std::ifstream::failbit ) != 0 )
		    std::cerr << msg << caminho << endl;
}

void parse(){
	ifstream arquivo;
	arquivo.open("etc/Operacao.alg");
	if ( (arquivo.rdstate() & std::ifstream::failbit ) != 0 )
		    std::cerr << "Erro abrindo o arquivo de Operacoes Algebricas.\n";

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
