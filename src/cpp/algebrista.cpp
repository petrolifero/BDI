#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>
#include <cassert>
#include "parserAlgebrico.cpp"
#include <stdio.h>

using namespace std;
using namespace boost;


/*
 *	Execução das operações aritméticas
 *
 * */
void executarProjecao(string linha){

	// Parâmetros da operação algébrica de projeção
	vector<string> parametros = separarParametros(linha);

	//std::copy(parametros.begin(), parametros.end(), std::ostream_iterator<string>(std::cout, "\n"));

	// nomes de parâmetros em particular
	string 	nomeTabela = parametros[0],
			nomeCatalogo = nome_ctl(nomeTabela),
			nomeDados = nome_dad(nomeTabela);

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
		   nomeResCtl(nome_ctl(nomeResultante)),
		   nomeResDad(nome_dad(nomeResultante));

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
	abrir(projCtl, nomeResCtl.c_str(), fstream::out, "ERRO: Problema na criação do arquivo: \n");

	inCtl >> inAtr;
	
	int rc;

	assert(2 == sscanf(inAtr.c_str(), "%d,%d", &inGrau, &inCard) && "Erro na leitura da cardinalidade e grau.\n");

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
	cout << "\nArquivo CTL da projeção:\n";
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

	// TODO: verificar necessidade de colocar isto em "abrir"
	inDad.imbue(locale(inDad.getloc(), new novalinha));
	cout << "\nArquivo DAD da projeção:\n";

	for(int i = 0; i < inCard; i++){
		inDad >> inVal;
		inVals = split(inVal, ',');

		//std::for_each(projCtlCols.begin(), projCtlCols.end(), EscreverMapa<map<int, string>::value_type>(std::cout));
		//std::copy(inVals.begin(), inVals.end(), std::ostream_iterator<string>(std::cout, " "));
		for(auto const& par : projCtlCols) {
			projDad << inVals[par.first] << " ";
			cout << inVals[par.first] << " ";
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
		rc = sscanf(valAtr.c_str(), "%d", &valorAtr);
		assert(rc == 1 && "erro na leitura do valor na relação.\n");
		rc = sscanf(val.c_str(), "%d", &valor);
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
		   nomeCatalogo = nome_ctl(nomeTabela),
		   nomeDados = nome_dad(nomeTabela),

		   nomeAtr(parametros[1]),
		   operador(parametros[2]),
		   valor(parametros[3]),

		   nomeFinal(parametros[4]),
		   selNomeCtl(nome_ctl(nomeFinal)),
		   selNomeDad(nome_dad(nomeFinal));

	fstream inCtl,
			inDad,
			selCtl,
			selDad;

	int inGrau,
		inCard,
		selCard = 0,
		selInd; //índice do atributo sendo selecionado

	string inAtr,
		   inVal,
		   selTipo;

	abrir(inCtl, nomeCatalogo.c_str(), fstream::in, "ERRO: Catálogo da tabela inexistente.\n");
	abrir(inDad, nomeDados.c_str(), fstream::in, "ERRO: Arquivo de dados da tabela inexistente: \n");

	inCtl >> inAtr;

	assert(2 == sscanf(inAtr.c_str(), "%d,%d", &inGrau, &inCard) 
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
	inDad.imbue(locale(inDad.getloc(), new novalinha));
	cout << "\nArquivo DAD da Seleção: \n";

	vector<string> inVals;
	for(int i = 0; i < inCard; i++){
		inDad >> inVal;
		inVals = split(inVal, ',');

		if(_satisfaz(inVals[selInd], selTipo, operador, valor)){
			for(auto const& valor : inVals){
				selDad << valor << ",";
				cout << valor << ",";
			}

			selCard++;
			selDad << endl;
			cout << endl;
		}
	}

	selCtl << inGrau << "," << selCard << endl;

	//ctl da relação é igual à da seleção
	inCtl.seekg(0);
	inCtl >> inAtr;
	cout << "\nArquivo CTL da Seleção:\n";

	while(inCtl >> inAtr){
		selCtl << inAtr << endl;
		cout << inAtr << endl;
	}

	inCtl.close();
	inDad.close();
	selCtl.close();
	selDad.close();
}

/*
 *	TODO ajustar construção do CTL
 *		 ajustar construção do DAD
 * */
void executarJuncao(string linha){

	/*
	 *	Algoritmo de junção:
	 *
	 *	abrir arquivos 1 e 2
	 *	identificar atributo sendo usado como critério de junção
	 *	verificar se atributo está ordenado ou não
	 *	se não está ordenado,
	 *		ordenar linhas
	 * 	andar com iteradores sobre as linhas de cada tabela
	 * 		se tupla na 1ª e tupla na 2ª satisfazem critério,
	 * 			junção da tupla na 1ª com a tupla na 2º entra na tabela final
	 * 	fechar arquivo de junção
	 * 	fechar arquivos 1 e 2
	 *
	 */

	vector<string> parametros = separarParametros(linha);

	string nomeTabA(parametros[0]),
		   nomeTabB(parametros[1]),
		   cond(parametros[2]),
		   nomef(parametros[3]),

		   nomeCtlA = nome_ctl(nomeTabA),
		   nomeDadA = nome_dad(nomeTabA),

		   nomeCtlB = nome_ctl(nomeTabB),
		   nomeDadB = nome_dad(nomeTabB),

		   nomeCtlJun = nome_ctl(nomef),
		   nomeDadJun = nome_dad(nomef);

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
		cardJ = 0,
		indA,
		indB;

	bool ordenarA = false,
		 ordenarB = false;

	abrir(inCtlA, nomeCtlA.c_str(), fstream::in, 
			"Falha na abertura do 1º Arquivo de Catálogo: \n");
	abrir(inDadA, nomeDadA.c_str(), fstream::in, 
			"Falha na abertura do 2º Arquivo de Dados: \n");

	abrir(inCtlB, nomeCtlB.c_str(), fstream::in, 
			"Falha na abertura do 1º Arquivo de Catálogo: \n");
	abrir(inDadB, nomeDadB.c_str(), fstream::in, 
			"Falha na abertura do 2º Arquivo de Dados: \n");

	abrir(junCtl, nomeCtlJun.c_str(), fstream::out, 
			"Falha na criação do Arquivo de Catálogo: \n");
	abrir(junDad, nomeDadJun.c_str(), fstream::out, 
			"Falha na criação do Arquivo de dados: \n");

	
	string linhaCtlA, 
		   linhaCtlB;

	//Leitura da Linha com grau e cardinalidade
	inCtlA >> linhaCtlA;

	assert(2 == sscanf(linhaCtlA.c_str(), "%d,%d", &grauA, &cardA) 
			&& "Erro na leitura da cardinalidade e grau.\n");

	inCtlB >> linhaCtlB;

	assert(2 == sscanf(linhaCtlB.c_str(), "%d,%d", &grauB, &cardB) 
			&& "Erro na leitura da cardinalidade e grau.\n");

	//modificadores nas tabelas de ctl
	vector<string> mods;
	string tipoAtr;
	for(int i = 0; i < grauA; i++){
		inCtlA >> linhaCtlA;

		mods = split(linhaCtlA, ',');
		
		if(mods[0] == nomeAtrA){
			indA = i;
			tipoAtr = mods[1];

			if(find(mods.begin(), mods.end(), "ord") == mods.end()){
				ordenarA = true;
			}

			// achei o atributo que regerá a junção
		}
	}

	for(int i = 0; i < grauB; i++){
		inCtlB >> linhaCtlB;

		mods = split(linhaCtlB, ',');

		if(mods[0] == nomeAtrB){
			indB = i;
			tipoAtr = mods[1];

			if(find(mods.begin(), mods.end(), "ord") == mods.end()){
				ordenarB = true;
			}

			// achei o atributo que regerá a junção
		}
	}

	map<string, string> linhasA, linhasB;
	string linhaA, linhaB;
	vector<string> inVals;

	inDadA.imbue(locale(inDadA.getloc(), new novalinha));
	inDadB.imbue(locale(inDadB.getloc(), new novalinha));

	for(int i = 0; i < cardA; i++){
		inDadA >> linhaA;
		inVals = split(linhaA, ',');
		linhasA.insert(make_pair(inVals[indA], linhaA));
	}

	for(int i = 0; i < cardB; i++){
		inDadB >> linhaB;
		inVals = split(linhaB, ',');
		linhasB.insert(make_pair(inVals[indB], linhaB));
	}
	
	map<string, string>::iterator itArqA(linhasA.begin()),
								  itArqB(linhasB.begin());

	cout << "\nArquivo DAD da junção:\n";

	while(itArqA != linhasA.end() 
			&& itArqB != linhasB.end()){

		if(_satisfaz(itArqA->first, tipoAtr, operador, itArqB->first)){
			junDad << itArqA->second << "," << itArqB->second << endl;
			cout << itArqA->second << "," << itArqB->second << endl;
			cardJ++;
		}

		if(itArqA->first > itArqB->first){
			itArqB++;
		} else if(itArqB->first > itArqA->first){
			itArqA++;
		} else {
			itArqA++;
			itArqB++;
		}
	}

	grauJ = grauA + grauB;
	cout << "\nArquivo CTL da junção:\n";

	junCtl << grauJ << "," << cardJ << endl;
	cout << grauJ << "," << cardJ << endl;

	inCtlA.seekg(0);
	inCtlA >> linhaA;

	inCtlB.seekg(0);
	inCtlB >> linhaB;
	
	for(int i = 0; i < grauJ; i++){
		if(i < grauA){
			inCtlA >> linhaA;
			cout << linhaA << endl;
			junCtl << linhaA << endl;
		} else {
			inCtlB >> linhaB;
			cout << linhaB << endl;
			junCtl << linhaB << endl;
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
	abrir(arquivo, "temp/Operacao.alg");
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
