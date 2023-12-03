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

#define MAX_POKEMONES 3
#define MAX_NOMBRE 20
struct jugador {
	lista_t *lista_pokemones;
	pokemon_t *pokemones[MAX_POKEMONES];
};

jugador_t *jugador_crear(lista_t *pokemon)
{
	jugador_t *jugador = calloc(1, sizeof(jugador_t));

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

bool jugador_pokemon_existe(jugador_t *jugador, char *pokemon)
{
	for (int i = 0; i < MAX_POKEMONES; i++) {
		if (jugador->pokemones[i] == NULL)
			break;
		if (strcmp(pokemon_nombre(jugador->pokemones[i]), pokemon) == 0)
			return true;
	}
	return false;
}

bool jugador_seleccionar_pokemon(jugador_t *jugador, char **nombre1,
				 char **nombre2, char **nombre3)
{
	for (int i = 0; i < MAX_POKEMONES; i++) {
		char linea[MAX_NOMBRE];
		printf("Elija un pokemon para agregar a tu mazo: ");
		fgets(linea, sizeof(linea), stdin);
		linea[strcspn(linea, "\n")] = '\0';
		printf("------------------------------------------------------------------------------------------------------------------\n");
		pokemon_t *pokemon = lista_buscar_elemento(
			jugador->lista_pokemones, jugador_comparador, linea);
		while (!pokemon || jugador_pokemon_existe(jugador, linea)) {
			printf("El pokemon ingresado no existe en la lista o ya pertenece a tu mazo, ingrese otro: ");
			fgets(linea, sizeof(linea), stdin);
			linea[strcspn(linea, "\n")] = '\0';
			printf("------------------------------------------------------------------------------------------------------------------\n");
			pokemon = lista_buscar_elemento(
				jugador->lista_pokemones, jugador_comparador,
				linea);
		}

		if (i == 0) {
			*nombre1 = strdup2(linea);
			jugador->pokemones[i] = pokemon;
		} else if (i == 1) {
			*nombre2 = strdup2(linea);
			jugador->pokemones[i] = pokemon;
		} else if (i == 2) {
			*nombre3 = strdup2(linea);
			jugador->pokemones[i] = pokemon;
		}
	}
	return true;
}

void jugador_tercer_pokemon(jugador_t *jugador, char *nombre3)
{
	jugador->pokemones[2] = lista_buscar_elemento(
		jugador->lista_pokemones, jugador_comparador, nombre3);
}

jugada_t jugador_pedir_nombre_y_ataque(jugador_t *jugador)
{
	char linea[MAX_NOMBRE];
	char *nombre = malloc(sizeof(char) * MAX_NOMBRE);
	char *ataque = malloc(sizeof(char) * MAX_NOMBRE);

	printf("Ingrese el nombre del pokemon para atacar: ");
	fgets(linea, sizeof(linea), stdin);
	linea[strcspn(linea, "\n")] = '\0';
	strcpy(nombre, linea);
	printf("------------------------------------------------------------------------------------------------------------------\n");
	while (!jugador_pokemon_existe(jugador, nombre)) {
		printf("El pokemon ingresado no existe en tu mazo, ingrese otro: ");
		fgets(linea, sizeof(linea), stdin);
		linea[strcspn(linea, "\n")] = '\0';
		strcpy(nombre, linea);
		printf("------------------------------------------------------------------------------------------------------------------\n");
	}

	printf("Ingrese el nombre del ataque: ");
	fgets(linea, sizeof(linea), stdin);
	linea[strcspn(linea, "\n")] = '\0';
	strcpy(ataque, linea);
	printf("------------------------------------------------------------------------------------------------------------------\n");

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

	lista_destruir(jugador->lista_pokemones);

	free(jugador);
}