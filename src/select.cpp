#include <stdio.h>
#include <cassert>
#include <fstream>
#include <iterator>
#include <errno.h>
#include "algebrista.cpp"

using namespace boost;
using namespace std;

void identificarComando(string &comando);

/**
 *	SQL
 * */

// Função de interface com código em C
// obtém comando do arquivo, e passa para o manipulador do select
// PRE: arquivo está posicionado num SELECT
extern "C" void sql_select(FILE *ponteiroArquivo){
	assert(ponteiroArquivo != NULL && "Por favor especifique um arquivo SQL existente.");
	
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
				// TODO: implementar possibilidade de nome da tabela antes do nome do atributo
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

/*int main(int argc, char **argv){
	FILE *arq = fopen(argv[1], "r");
	select(arq);
	select(arq);
	select(arq);
	select(arq);
	select(arq);

	parse();

	fclose(arq);
}*/
