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

struct adversario {
	lista_t *lista_pokemones;
	pokemon_t *pokemones[MAX_POKEMONES];
	lista_t *ataques_totales;
	size_t ultimo_indice_ataque;
};

adversario_t *adversario_crear(lista_t *pokemon)
{
	adversario_t *adversario = calloc(1, sizeof(adversario_t));

	if (adversario == NULL)
		return NULL;

	adversario->lista_pokemones = pokemon;
	adversario->ataques_totales = lista_crear();

	return adversario;
}

char *strdup(const char *s)
{
	char *d = malloc(strlen(s) + 1);

	if (d == NULL)
		return NULL;

	strcpy(d, s);

	return d;
}

void agregar_ataques_totales(const struct ataque *ataque, void *aux)
{
	adversario_t *adversario = (adversario_t *)aux;
	lista_insertar(adversario->ataques_totales, (void *)ataque);
}

bool adversario_seleccionar_pokemon(adversario_t *adversario, char **nombre1,
				    char **nombre2, char **nombre3)
{
	if (lista_vacia(adversario->lista_pokemones) || adversario == NULL)
		return false;

	srand((unsigned int)time(NULL));

	for (int i = 0; i < MAX_POKEMONES; i++) {
		size_t random = (size_t)rand() %
				lista_tamanio(adversario->lista_pokemones);
		pokemon_t *pokemon = lista_elemento_en_posicion(
			adversario->lista_pokemones, random);

		switch (i) {
		case 0:
			*nombre1 = strdup(pokemon_nombre(pokemon));
			adversario->pokemones[0] = pokemon;
			printf("El pokemon elegido del adversario es: %s\n",
			       *nombre1);
			printf("------------------------------------------------------------------------------------------------------------------\n");
			con_cada_ataque(pokemon, agregar_ataques_totales,
					adversario);
			break;
		case 1:
			while (strcmp(pokemon_nombre(pokemon), *nombre1) == 0) {
				random = (size_t)rand() %
					 lista_tamanio(
						 adversario->lista_pokemones);
				pokemon = lista_elemento_en_posicion(
					adversario->lista_pokemones, random);
			}
			*nombre2 = strdup(pokemon_nombre(pokemon));
			adversario->pokemones[1] = pokemon;
			printf("El pokemon elegido del adversario es: %s\n",
			       *nombre2);
			printf("------------------------------------------------------------------------------------------------------------------\n");
			con_cada_ataque(pokemon, agregar_ataques_totales,
					adversario);
			break;
		case 2:
			while (strcmp(pokemon_nombre(pokemon), *nombre1) == 0 ||
			       strcmp(pokemon_nombre(pokemon), *nombre2) == 0) {
				random = (size_t)rand() %
					 lista_tamanio(
						 adversario->lista_pokemones);
				pokemon = lista_elemento_en_posicion(
					adversario->lista_pokemones, random);
			}
			*nombre3 = strdup(pokemon_nombre(pokemon));
			adversario->pokemones[2] = pokemon;
			printf("El pokemon elegido del adversario es: %s\n",
			       *nombre3);
			printf("------------------------------------------------------------------------------------------------------------------\n");
			break;
		default:
			break;
		}
	}

	return true;
}

int comparador(void *pokemon1, void *pokemon2)
{
	return strcmp(pokemon_nombre(pokemon1), pokemon_nombre(pokemon2));
}

bool adversario_pokemon_seleccionado(adversario_t *adversario, char *nombre1,
				     char *nombre2, char *nombre3)
{
	if (adversario == NULL)
		return false;

	pokemon_t *pokemon = lista_buscar_elemento(
		adversario->lista_pokemones,
		(int (*)(void *, void *))comparador, nombre3);

	adversario->pokemones[2] = pokemon;

	if (adversario->pokemones[2] == NULL)
		return false;

	con_cada_ataque(pokemon, agregar_ataques_totales, adversario);

	printf("Los pokemones del adversario son: %s, %s y %s\n",
	       pokemon_nombre(adversario->pokemones[0]),
	       pokemon_nombre(adversario->pokemones[1]),
	       pokemon_nombre(pokemon));
	printf("------------------------------------------------------------------------------------------------------------------\n");

	return true;
}

bool ataque_pokemon(void *elemento, void *aux)
{
	const char *ataque = (const char *)elemento;
	pokemon_t *pokemon = (pokemon_t *)aux;

	if (pokemon_buscar_ataque(pokemon, ataque) != NULL) {
		return true;
	}
	return false;
}

jugada_t adversario_proxima_jugada(adversario_t *adversario)
{
	if (adversario == NULL)
		return (jugada_t){ .ataque = "", .pokemon = "" };

	srand((unsigned int)time(NULL));
	size_t tamanio = lista_tamanio(adversario->ataques_totales);
	int pokemon_random = rand() % MAX_POKEMONES;
	size_t ataque_random = (size_t)rand() % tamanio;

	pokemon_t *pokemon = adversario->pokemones[pokemon_random];
	char *ataque = lista_elemento_en_posicion(adversario->ataques_totales,
						  ataque_random);

	while (!ataque_pokemon(ataque, pokemon) ||
	       !lista_buscar_elemento(adversario->ataques_totales,
				      (int (*)(void *, void *))comparador,
				      ataque)) {
		pokemon_random = rand() % MAX_POKEMONES;
		pokemon = adversario->pokemones[pokemon_random];
	}

	jugada_t j;
	strncpy(j.ataque, ataque, sizeof(j.ataque) - 1);
	strncpy(j.pokemon, pokemon_nombre(pokemon), sizeof(j.pokemon) - 1);

	j.ataque[sizeof(j.ataque) - 1] = '\0';
	j.pokemon[sizeof(j.pokemon) - 1] = '\0';

	adversario->ultimo_indice_ataque = ataque_random;

	return j;
}

void adversario_informar_jugada(adversario_t *a, jugada_t j)
{
	if (a == NULL)
		return;

	printf("El jugador eligió el ataque %s del pokemon %s\n", j.ataque,
	       j.pokemon);
	printf("------------------------------------------------------------------------------------------------------------------\n");

	lista_quitar_de_posicion(a->ataques_totales, a->ultimo_indice_ataque);

	return;
}

void adversario_destruir(adversario_t *adversario)
{
	if (adversario == NULL)
		return;

	lista_destruir(adversario->ataques_totales);

	lista_destruir(adversario->lista_pokemones);

	free(adversario);
}
