#include "lista.h"
#include <stddef.h>
#include <stdlib.h>

typedef struct nodo {
	void *elemento;
	struct nodo *siguiente;
} nodo_t;

struct lista {
	nodo_t *nodo_inicio;
	nodo_t *nodo_fin;
	size_t longitud;
};

struct lista_iterador {
	nodo_t *nodo_actual;
	lista_t *lista;
};

lista_t *lista_crear()
{
	lista_t *lista = calloc(1, sizeof(struct lista));

	if (!lista)
		return NULL;

	return lista;
}

lista_t *lista_insertar(lista_t *lista, void *elemento)
{
	if (!lista)
		return NULL;

	nodo_t *nodo_nuevo = calloc(1, sizeof(nodo_t));

	if (!nodo_nuevo)
		return NULL;

	nodo_nuevo->elemento = elemento;

	if (!lista->nodo_inicio) {
		lista->nodo_inicio = nodo_nuevo;
	} else {
		lista->nodo_fin->siguiente = nodo_nuevo;
	}

	lista->nodo_fin = nodo_nuevo;
	lista->longitud++;

	return lista;
}

lista_t *lista_insertar_en_posicion(lista_t *lista, void *elemento,
				    size_t posicion)
{
	if (!lista)
		return NULL;

	if (posicion >= lista->longitud)
		return lista_insertar(lista, elemento);

	nodo_t *nodo_nuevo = calloc(1, sizeof(nodo_t));

	if (!nodo_nuevo)
		return NULL;

	nodo_nuevo->elemento = elemento;

	if (!lista->nodo_inicio) {
		lista->nodo_inicio = nodo_nuevo;
	} else if (posicion == 0) {
		nodo_nuevo->siguiente = lista->nodo_inicio;
		lista->nodo_inicio = nodo_nuevo;
	} else {
		nodo_t *nodo_actual = lista->nodo_inicio;
		size_t i = 0;

		while (nodo_actual->siguiente && i < posicion - 1) {
			nodo_actual = nodo_actual->siguiente;
			i++;
		}

		nodo_nuevo->siguiente = nodo_actual->siguiente;
		nodo_actual->siguiente = nodo_nuevo;
	}

	lista->longitud++;

	return lista;
}

void *lista_quitar(lista_t *lista)
{
	if (!lista || !lista->nodo_inicio)
		return NULL;

	if (!lista->nodo_inicio->siguiente) {
		void *elemento = lista->nodo_inicio->elemento;
		free(lista->nodo_inicio);
		lista->nodo_inicio = NULL;
		lista->longitud--;
		return elemento;
	}

	nodo_t *nodo_actual = lista->nodo_inicio->siguiente;
	nodo_t *nodo_anterior = lista->nodo_inicio;

	while (nodo_actual->siguiente) {
		nodo_anterior = nodo_actual;
		nodo_actual = nodo_actual->siguiente;
	}

	void *elemento = nodo_actual->elemento;
	free(nodo_actual);
	nodo_anterior->siguiente = NULL;
	lista->nodo_fin = nodo_anterior;
	lista->longitud--;

	return elemento;
}

void *lista_quitar_de_posicion(lista_t *lista, size_t posicion)
{
	if (!lista || !lista->nodo_inicio)
		return NULL;

	if (posicion >= lista->longitud + 1)
		return lista_quitar(lista);

	nodo_t *nodo_aux = NULL;

	if (posicion == 0) {
		nodo_aux = lista->nodo_inicio;
		lista->nodo_inicio = nodo_aux->siguiente;
	} else {
		nodo_t *nodo_actual = lista->nodo_inicio;
		size_t i = 0;

		while (nodo_actual->siguiente && i < posicion - 1) {
			nodo_actual = nodo_actual->siguiente;
			i++;
		}

		nodo_aux = nodo_actual->siguiente;
		nodo_actual->siguiente = nodo_aux->siguiente;
	}

	void *elemento = nodo_aux->elemento;
	free(nodo_aux);
	lista->longitud--;

	return elemento;
}

void *lista_elemento_en_posicion(lista_t *lista, size_t posicion)
{
	if (!lista || !lista->nodo_inicio || posicion >= lista->longitud)
		return NULL;

	nodo_t *nodo_actual = lista->nodo_inicio;

	for (size_t i = 0; i < posicion; i++)
		nodo_actual = nodo_actual->siguiente;

	return nodo_actual->elemento;
}

void *lista_buscar_elemento(lista_t *lista, int (*comparador)(void *, void *),
			    void *contexto)
{
	if (!lista || !lista->nodo_inicio || !comparador)
		return NULL;

	nodo_t *nodo_actual = lista->nodo_inicio;

	while (nodo_actual->siguiente) {
		if (comparador(nodo_actual->elemento, contexto) == 0)
			return nodo_actual->elemento;
		nodo_actual = nodo_actual->siguiente;
	}

	if (comparador(nodo_actual->elemento, contexto) == 0)
		return nodo_actual->elemento;

	return NULL;
}

void *lista_primero(lista_t *lista)
{
	if (!lista || !lista->nodo_inicio)
		return NULL;
	return lista->nodo_inicio->elemento;
}

void *lista_ultimo(lista_t *lista)
{
	if (!lista || !lista->nodo_inicio)
		return NULL;

	nodo_t *nodo_actual = lista->nodo_inicio;

	while (nodo_actual->siguiente)
		nodo_actual = nodo_actual->siguiente;

	return nodo_actual->elemento;
}

bool lista_vacia(lista_t *lista)
{
	if (!lista)
		return true;

	return lista_tamanio(lista) == 0;
}

size_t lista_tamanio(lista_t *lista)
{
	if (!lista || !lista->nodo_inicio)
		return 0;

	return lista->longitud;
}

void lista_destruir(lista_t *lista)
{
	if (!lista)
		return;

	nodo_t *nodo_actual = lista->nodo_inicio;

	while (nodo_actual) {
		nodo_t *nodo_aux = nodo_actual->siguiente;
		free(nodo_actual);
		nodo_actual = nodo_aux;
	}

	free(lista);
}

void lista_destruir_todo(lista_t *lista, void (*funcion)(void *))
{
	if (!lista)
		return;

	nodo_t *nodo_actual = lista->nodo_inicio;

	while (nodo_actual) {
		nodo_t *nodo_aux = nodo_actual->siguiente;

		if (funcion && nodo_actual->elemento) {
			funcion(nodo_actual->elemento);
		}

		free(nodo_actual);
		nodo_actual = nodo_aux;
	}

	free(lista);

	return;
}

lista_iterador_t *lista_iterador_crear(lista_t *lista)
{
	if (!lista)
		return NULL;

	lista_iterador_t *iterador = calloc(1, sizeof(lista_iterador_t));

	if (!iterador)
		return NULL;

	iterador->lista = lista;

	if (lista->nodo_inicio)
		iterador->nodo_actual = lista->nodo_inicio;

	return iterador;
}

bool lista_iterador_tiene_siguiente(lista_iterador_t *iterador)
{
	if (!iterador || !iterador->lista || !iterador->nodo_actual)
		return false;
	else
		return true;
}

bool lista_iterador_avanzar(lista_iterador_t *iterador)
{
	if (!iterador || !iterador->lista || !iterador->nodo_actual)
		return false;
	iterador->nodo_actual = iterador->nodo_actual->siguiente;
	if (!iterador->nodo_actual)
		return false;
	return true;
}

void *lista_iterador_elemento_actual(lista_iterador_t *iterador)
{
	if (!iterador || !iterador->lista)
		return NULL;

	if (iterador->nodo_actual)
		return iterador->nodo_actual->elemento;

	return NULL;
}

void lista_iterador_destruir(lista_iterador_t *iterador)
{
	if (!iterador)
		return;

	free(iterador);
}

size_t lista_con_cada_elemento(lista_t *lista, bool (*funcion)(void *, void *),
			       void *contexto)
{
	if (!lista || !lista->nodo_inicio || !funcion)
		return 0;

	nodo_t *nodo_actual = lista->nodo_inicio;
	size_t elementos_recorridos = 0;

	while (nodo_actual) {
		elementos_recorridos++;
		if (!funcion(nodo_actual->elemento, contexto))
			return elementos_recorridos;
		nodo_actual = nodo_actual->siguiente;
	}

	return elementos_recorridos;
}