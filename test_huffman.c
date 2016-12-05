#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#define BEFORE_TEST before();

#include <limits.h>
#include <stdlib.h>

#include "ufftest.h"

#include "no.h"
#include "ref.h"
#include "cod.h"
#include "heap.h"
#include "huffman.h"

void before()
{
}

BEGIN_TESTS();

TEST("Criação e destruição de nós, estrutura fundamental");
if (!skip) {
	No *vazio = no_criar(0, 'a', NULL, NULL, 0);

	ASSERT_NOT_EQUAL(vazio, NULL);

	no_liberar(vazio, 0);

}

TEST("Criação de nós com filhos");
if (!skip) {

	No *filhoEsq = no_criar(1, 'a', NULL, NULL, 1),
	   *filhoDir = no_criar(1, 'b', NULL, NULL, 2),
	   *pai = no_criar(0, 'a', filhoEsq, filhoDir, 0);

	ASSERT_EQUAL(pai->fEsquerda->caminho, filhoEsq->caminho);

	no_liberar(pai, 1);
}

TEST("Criação de Referências");
if (!skip) {

	No *filhoEsq = no_criar(1, 'a', NULL, NULL, 1),
	   *filhoDir = no_criar(1, 'b', NULL, NULL, 2),
	   *pai = no_criar(0, 'a', filhoEsq, filhoDir, 0);

	Ref *r = ref_criar(pai, pai->rotulo);

	ASSERT_EQUAL(r->ref->fEsquerda->caminho, filhoEsq->caminho);

	ref_liberar(r, 2);
}

TEST("Criação de codificações");
if (!skip) {
	Codificacao **cods = cods_criar();

	int i = 0;
	for(i = 0; i < 6; i++){
		ASSERT_EQUAL(cods[i]->simbolo, 'A' + i);
		
	}

	cods_liberar(cods);
}

TEST("Criação da heap");
if (!skip) {
	
	HeapFrequencias *heap = heap_criar(4);

	ASSERT_NOT_EQUAL(heap, NULL);

	heap_liberar(heap, 2);

}

TEST("Adição de elementos à heap");
if (!skip) {
	HeapFrequencias *heap = heap_criar(4);

	No *filhoEsq = no_criar(1, 'a', NULL, NULL, 1),
	   *filhoDir = no_criar(1, 'b', NULL, NULL, 2),
	   *pai = no_criar(4, 'a', filhoEsq, filhoDir, 0);

	heap_subir(heap, pai);

	ASSERT_EQUAL(heap->fila[0]->rotulo, pai->rotulo);

	heap_liberar(heap, 2);

}

TEST("Inserção de multiplos nós na heap");
if (!skip) {
	HeapFrequencias *heap = heap_criar(4);

	No *filhoEsq = no_criar(1, 'a', NULL, NULL, 1),
	   *filhoDir = no_criar(1, 'b', NULL, NULL, 2),
	   *pai = no_criar(4, 'a', NULL, NULL, 0);

	heap_subir(heap, pai);
	heap_subir(heap, filhoDir);
	heap_subir(heap, filhoEsq);

	ASSERT_EQUAL(heap->fila[0]->rotulo, filhoDir->rotulo);
	ASSERT_EQUAL(heap->fila[1]->rotulo, pai->rotulo);
	ASSERT_EQUAL(heap->fila[2]->rotulo, filhoEsq->rotulo);

	heap_liberar(heap, 2);
}

TEST("Remoção de nós da heap");
if (!skip) {
	HeapFrequencias *heap = heap_criar(4);

	No *filhoEsq = no_criar(1, 'a', NULL, NULL, 1),
	   *filhoDir = no_criar(1, 'b', NULL, NULL, 2),
	   *pai = no_criar(4, 'a', NULL, NULL, 0);

	heap_subir(heap, pai);
	heap_subir(heap, filhoDir);
	heap_subir(heap, filhoEsq);

	No *filhoRemovido = heap_descer(heap);

	ASSERT_EQUAL(filhoEsq->rotulo, filhoRemovido->rotulo);

	heap_liberar(heap, 2);
	no_liberar(filhoRemovido, 2);
}

TEST("Teste do algoritmo de Huffman");
if (!skip) {
	
	char *fonte = "BBBCCCCDEFFFBAAAAAAAAAACDDFFFFFFFCF";
	
	Codificacao **tabela = cods_criar();

	huffman(fonte, tabela);
	cods_liberar(tabela);
}

END_TESTS();
