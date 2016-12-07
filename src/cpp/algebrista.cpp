#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <boost/range/algorithm_ext/erase.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <iostream>
#include <cassert>

using namespace std;
using namespace boost;

/**
 *	Separação de Strings em vetores
 */

void abrir(fstream &arquivo, const char* caminho, ios_base::openmode mode = fstream::in, 
		string msg = "Erro abrindo o arquivo: "){

	arquivo.open(caminho, mode);
	if ( (arquivo.rdstate() & std::ifstream::failbit ) != 0 ){
		std::cerr << msg << caminho << endl;
		exit(1);
	}
}

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

string parseAtr(string where){
	size_t found = where.find_first_of("=<>");	

	return where.substr(0, found);
}

string parseFirstAtrInTab(string where){
	size_t found = where.find_first_of(".");

	return parseAtr(where.substr(found));
}

string parseLastAtrInTab(string where){
	size_t found = where.find_last_of(".");

	return where.substr(found);
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

/**
 *	TODO Proposta: caminho como entrada da função
 *
 *	@return nome da projeção gerada, ou "" caso a relação de entrada seja inválida
 * */
string escreverProjecao(string relacao, int qtdAtributos, string listaAtributos, string nomeProjecao){
	if(relacao.empty()) return "";

	fstream arquivo;
	abrir(arquivo, "etc/Operacao.alg", fstream::app);

	arquivo << "P(" << relacao << "," << qtdAtributos << "," << listaAtributos << "," << nomeProjecao << ")\n";

	arquivo.close();

	return nomeProjecao;
}

/**
 *	TODO Proposta: caminho como entrada da função
 * */
string escreverSelecao(string relacao, string atr, string op, string valor, string nomeSelecao){
	fstream arquivo;
	abrir(arquivo, "etc/Operacao.alg", fstream::app);
	arquivo << "S(" << relacao << "," << atr << "," << op << "," << valor << ", " << nomeSelecao << ")\n";
	arquivo.close();

	return nomeSelecao;
}

/**
 *	TODO Proposta: caminho como entrada da função
 * */
string escreverJuncao(string relacao, string relacaoB, string condicao, string nomeJuncao){
	fstream arquivo;
	abrir(arquivo, "etc/Operacao.alg", fstream::app);

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

	fstream inCtl,//ctl da entrada
			 inDad,//dad da entrada
			 projCtl,//Ctl da projeção
			 projDad;//dad da projeção

	for(int i = 2; i < 2 + tam; i++){
		projAtrs.push_back(parametros[i]);
	}
	
	// 1. Ler arquivo de catálogo
	abrir(inCtl, nomeCatalogo.c_str(), fstream::in, "ERRO: Catálogo da tabela inexistente.\n");

	// 2. Criar arquivo de catálogo final
	abrir(projCtl, nomeCatalogo.c_str(), fstream::out, "ERRO: Problema na criação do arquivo: \n");

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
	abrir(projDad, nomeResDad.c_str(), fstream::out, "ERRO: Falha na criação do arquivo: \n");
	// Iterar sobre os índices no mapa, só obtendo do DAD da relação aqueles valores dos índices passados
	abrir(inDad, nomeDados.c_str(), fstream::in, "ERRO: Arquivo de dados da tabela inexistente: \n");

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

	fstream inCtl,
			inDad,
			selCtl,
			selDad;

	int inGrau,
		inCard,
		selCard,
		selInd; //índice do atributo sendo selecionado

	string inAtr,
		   inVal,
		   selTipo;

	abrir(inCtl, nomeCatalogo.c_str(), fstream::in, "ERRO: Catálogo da tabela inexistente.\n");
	abrir(inDad, nomeDados.c_str(), fstream::in, "ERRO: Arquivo de dados da tabela inexistente: \n");

	inCtl >> inAtr;

	assert(2 == scanf(inAtr.c_str(), "%d,%d\n", &inGrau, &inCard) 
			&& "Erro na leitura da cardinalidade e grau.\n");

	abrir(selCtl, selNomeCtl.c_str(), fstream::out, "ERRO: Falha na criação do arquivo: \n");
	abrir(selDad, selNomeDad.c_str(), fstream::out, "ERRO: Falha na criação do arquivo: \n");

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

	vector<string> parametros = separarParametros(linha);

	string nomeTabA(parametros[0]),
		   nomeTabB(parametros[1]),
		   cond(parametros[2]),
		   nomef(parametros[3]),

		   nomeCtlA = nomeTabA + ".ctl",
		   nomeDadA = nomeTabA + ".dad",
		   nomeCtlB = nomeTabB + ".ctl",
		   nomeDadB = nomeTabB + ".dad",

		   nomeCtlJun = nomef + ".ctl",
		   nomeDadJun = nomef + ".dad";

	string nomeAtrA = parseFirstAtrInTab(cond), 
		   operador = parseOp(cond),
		   nomeAtrB = parseLastAtrInTab(cond);

	fstream inCtlA,
			inDadA,
			inCtlB,
			inDadB,
			junCtl,
			junDad;

	int grauA,
		grauB,
		grauJ,
		cardA,
		cardB,
		cardJ,
		indA,
		indB;

	abrir(inCtlA, nomeCtlA.c_str(), fstream::in, "Falha na abertura do 1º Arquivo de Catálogo: \n");
	abrir(inDadA, nomeDadA.c_str(), fstream::in, "Falha na abertura do 2º Arquivo de Dados: \n");
	abrir(inCtlB, nomeCtlB.c_str(), fstream::in, "Falha na abertura do 1º Arquivo de Catálogo: \n");
	abrir(inDadB, nomeDadB.c_str(), fstream::in, "Falha na abertura do 2º Arquivo de Dados: \n");

	abrir(junCtl, nomeCtlJun.c_str(), fstream::out, "Falha na criação do Arquivo de Catálogo: \n");
	abrir(junDad, nomeDadJun.c_str(), fstream::out, "Falha na criação do Arquivo de dados: \n");

	// TODO: Ler ctl's, descobrir colunas de verificação da condição
	
	string linhaCtlA, 
		   linhaCtlB;

	inCtlA >> linhaCtlA;

	assert(2 == scanf(linhaCtlA.c_str(), "%d,%d\n", &grauA, &cardA) 
			&& "Erro na leitura da cardinalidade e grau.\n");

	inCtlB >> linhaCtlB;

	assert(2 == scanf(linhaCtlB.c_str(), "%d,%d\n", &grauB, &cardB) 
			&& "Erro na leitura da cardinalidade e grau.\n");

	grauJ = grauA + grauB;
	cardJ = (cardA > cardB) ? cardA : cardB;

	junCtl << grauJ << "," << cardJ << endl;

	vector<string> mods;
	for(int i = 0; i < grauA; i++){
		inCtlA >> linhaCtlA;

		junCtl << linhaCtlA;
		mods = split(linhaCtlA, ',');

		if(mods[0] == nomeAtrA){
			indA = i;
			// achei o atributo que regerá a junção
		}
	}

	for(int i = 0; i < grauB; i++){
		inCtlB >> linhaCtlB;

		junCtl << linhaCtlB;
		mods = split(linhaCtlB, ',');

		if(mods[0] == nomeAtrB){
			indB = i;
			// achei o atributo que regerá a junção
		}
	}

	vector<string> colunasA, colunasB;
	string linhaA, linhaB;

	bool lerA, lerB;

	for(int i = 0; i < cardJ; i++){
		if(lerA){
			inDadA >> linhaA;
			colunasA = split(linhaA, ' ');
		} else {
			
		}
		
		if(lerB) {
			inDadB >> linhaB;
			colunasB = split(linhaB, ' ');
		} else {
		
		}



	}

	inCtlA.close();
	inDadA.close();
	inCtlB.close();
	inDadB.close();
	junCtl.close();
	junDad.close();
}

void parse(){
	fstream arquivo;
	abrir(arquivo, "etc/Operacao.alg");
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
