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

struct adversario {
	lista_t *lista_pokemones;
	pokemon_t *pokemones[3];
	lista_t *ataques_totales;
	abb_t *ataques;
};

adversario_t *adversario_crear(lista_t *pokemon)
{
	adversario_t *adversario = malloc(sizeof(adversario_t));

	if (adversario == NULL)
		return NULL;

	adversario->lista_pokemones = pokemon;
	adversario->ataques_totales = lista_crear();
	adversario->ataques = abb_crear((abb_comparador)strcmp);

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

void agregar_ataque(const struct ataque *ataque, void *aux)
{
	abb_insertar((abb_t *)aux, (void *)ataque);
}

void agregar_ataques(abb_t *ataques, pokemon_t *pokemon)
{
	con_cada_ataque(pokemon, agregar_ataque, ataques);
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

	for (int i = 0; i < 3; i++) {
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
			agregar_ataques(adversario->ataques, pokemon);
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
			agregar_ataques(adversario->ataques, pokemon);
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

	agregar_ataques(adversario->ataques, pokemon);
	con_cada_ataque(pokemon, agregar_ataques_totales, adversario);

	nombre3 = strdup(pokemon_nombre(pokemon));

	printf("Los pokemones del adversario son: %s, %s y %s\n",
	       pokemon_nombre(adversario->pokemones[0]),
	       pokemon_nombre(adversario->pokemones[1]),
	       pokemon_nombre(pokemon));

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
	srand((unsigned int)time(NULL));
	size_t tamanio = abb_tamanio(adversario->ataques);
	int pokemon_random = rand() % 3;
	size_t ataque_random = (size_t)rand() % tamanio;

	pokemon_t *pokemon = adversario->pokemones[pokemon_random];
	char *ataque = lista_elemento_en_posicion(adversario->ataques_totales,
						  ataque_random);

	while (!ataque_pokemon(ataque, pokemon) ||
	       !abb_buscar(adversario->ataques, ataque)) {
		pokemon_random = rand() % 3;
		pokemon = adversario->pokemones[pokemon_random];
	}

	jugada_t j;
	strncpy(j.ataque, ataque, sizeof(j.ataque) - 1);
	strncpy(j.pokemon, pokemon_nombre(pokemon), sizeof(j.pokemon) - 1);

	j.ataque[sizeof(j.ataque) - 1] = '\0';
	j.pokemon[sizeof(j.pokemon) - 1] = '\0';

	abb_quitar(adversario->ataques, ataque);
	lista_quitar_de_posicion(adversario->ataques_totales, ataque_random);

	return j;
}

void adversario_informar_jugada(adversario_t *a, jugada_t j)
{
	printf("El jugador eligió el ataque %s del pokemon %s\n", j.ataque,
	       j.pokemon);

	return;
}

void adversario_destruir(adversario_t *adversario)
{
	if (adversario == NULL)
		return;

	abb_destruir(adversario->ataques);
	lista_destruir(adversario->ataques_totales);

	for (int i = 0; i < 3; i++) {
		free(adversario->pokemones[i]);
	}

	free(adversario);
}
