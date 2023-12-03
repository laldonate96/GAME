#include "jugador.h"
#include "adversario.h"
#include "juego.h"
#include "lista.h"
#include "pokemon.h"
#include "abb.h"
#include "ataque.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

struct jugador {
	lista_t *lista_pokemones;
	pokemon_t *pokemones[3];
};

jugador_t *jugador_crear(lista_t *pokemon)
{
	jugador_t *jugador = malloc(sizeof(jugador_t));

	if (jugador == NULL)
		return NULL;

	jugador->lista_pokemones = pokemon;

	return jugador;
}

char *strdup2(const char *s)
{
	char *d = malloc(strlen(s) + 1);

	if (d == NULL)
		return NULL;

	strcpy(d, s);

	return d;
}

int jugador_comparador(void *nombre1, void *nombre2)
{
	return strcmp(nombre1, nombre2);
}

bool jugador_seleccionar_pokemon(jugador_t *jugador, char **nombre1,
				 char **nombre2, char **nombre3)
{
	for (int i = 0; i < 3; i++) {
		char linea[20];
		printf("Elija un pokemon para agregar a tu mazo: ");
		fgets(linea, sizeof(linea), stdin);
		linea[strcspn(linea, "\n")] = '\0';

		while (lista_buscar_elemento(jugador->lista_pokemones,
					     jugador_comparador,
					     linea) == NULL) {
			printf("El pokemon ingresado no existe en la lista, ingrese otro: ");
			fgets(linea, sizeof(linea), stdin);
			linea[strcspn(linea, "\n")] = '\0';
		}

		if (i == 0) {
			*nombre1 = strdup2(linea);
			jugador->pokemones[i] = lista_buscar_elemento(
				jugador->lista_pokemones, jugador_comparador,
				*nombre1);
		} else if (i == 1) {
			*nombre2 = strdup2(linea);
			strcpy(*nombre2, linea);
			jugador->pokemones[i] = lista_buscar_elemento(
				jugador->lista_pokemones, jugador_comparador,
				*nombre2);
		} else if (i == 2) {
			*nombre3 = strdup2(linea);
			jugador->pokemones[i] = lista_buscar_elemento(
				jugador->lista_pokemones, jugador_comparador,
				*nombre3);
		}
	}
	return true;
}

void jugador_tercer_pokemon(jugador_t *jugador, char *nombre3)
{
	jugador->pokemones[2] = lista_buscar_elemento(
		jugador->lista_pokemones, jugador_comparador, nombre3);
}

bool jugador_pokemon_existe(jugador_t *jugador, char *pokemon)
{
	for (int i = 0; i < 3; i++) {
		if (strcmp(pokemon_nombre(jugador->pokemones[i]), pokemon) == 0)
			return true;
	}
	return false;
}

jugada_t jugador_pedir_nombre_y_ataque(jugador_t *jugador)
{
	char linea[20];
	char *nombre = malloc(sizeof(char) * 20);
	char *ataque = malloc(sizeof(char) * 20);

	printf("Ingrese el nombre del pokemon para atacar: ");
	fgets(linea, sizeof(linea), stdin);
	linea[strcspn(linea, "\n")] = '\0';
	strcpy(nombre, linea);
	while (!jugador_pokemon_existe(jugador, nombre)) {
		printf("El pokemon ingresado no existe en tu mazo, ingrese otro: ");
		fgets(linea, sizeof(linea), stdin);
		linea[strcspn(linea, "\n")] = '\0';
		strcpy(nombre, linea);
	}

	printf("Ingrese el nombre del ataque: ");
	fgets(linea, sizeof(linea), stdin);
	linea[strcspn(linea, "\n")] = '\0';
	strcpy(ataque, linea);

	jugada_t jugada = { "", "" };
	strcpy(jugada.pokemon, nombre);
	strcpy(jugada.ataque, ataque);

	free(nombre);
	free(ataque);

	return jugada;
}

void jugador_destruir(jugador_t *jugador)
{
	if (jugador == NULL)
		return;

	for (int i = 0; i < 3; i++) {
		free(jugador->pokemones[i]);
	}

	free(jugador);
}