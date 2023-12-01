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
	char *ataques_totales[9];
	abb_t *ataques;
};

adversario_t *adversario_crear(lista_t *pokemon)
{
	adversario_t *adversario = malloc(sizeof(adversario_t));

	if (adversario == NULL)
		return NULL;

	adversario->lista_pokemones = pokemon;
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

	for (int i = 0; i < 9; i++) {
		if (adversario->ataques_totales[i] == NULL) {
			adversario->ataques_totales[i] = strdup(ataque->nombre);
			break;
		}
	}
}

bool adversario_seleccionar_pokemon(adversario_t *adversario, char **nombre1,
				    char **nombre2, char **nombre3)
{
	if (lista_vacia(adversario->lista_pokemones) || adversario == NULL)
		return false;

	srand((unsigned int)time(NULL));

	for (int i = 0; i < 3; i++) {
		size_t random = (size_t)rand() % lista_tamanio(adversario->lista_pokemones);
		pokemon_t *pokemon = lista_elemento_en_posicion(
			adversario->lista_pokemones, random);

		switch (i)
		{
		case 0:
			*nombre1 = strdup(pokemon_nombre(pokemon));
			adversario->pokemones[0] = pokemon;
			printf("El pokemon elegido del adversario es: %s\n", *nombre1);
			agregar_ataques(adversario->ataques, pokemon);
			con_cada_ataque(pokemon, agregar_ataques_totales, adversario);
			break;
		case 1:
			*nombre2 = strdup(pokemon_nombre(pokemon));
			adversario->pokemones[1] = pokemon;
			printf("El pokemon elegido del adversario es: %s\n", *nombre2);
			agregar_ataques(adversario->ataques, pokemon);
			con_cada_ataque(pokemon, agregar_ataques_totales, adversario);
			break;
		case 2:
			*nombre3 = strdup(pokemon_nombre(pokemon));
			adversario->pokemones[2] = pokemon;
			printf("El pokemon elegido del adversario es: %s\n", *nombre3);
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
		adversario->lista_pokemones, (int (*)(void *, void *))comparador, nombre3);

	adversario->pokemones[2] = pokemon;

	if (adversario->pokemones[2] == NULL)
		return false;

	agregar_ataques(adversario->ataques, pokemon);
	con_cada_ataque(pokemon, agregar_ataques_totales, adversario);

	nombre3 = strdup(pokemon_nombre(pokemon));

	printf("Los pokemones del adversario son: %s, %s y %s\n", pokemon_nombre(adversario->pokemones[0]), pokemon_nombre(adversario->pokemones[1]),
	       pokemon_nombre(pokemon));

	return true;
}

bool ataque_pokemon(void *elemento, void *aux) {
    const char *ataque = (const char *)elemento;
    pokemon_t *pokemon = (pokemon_t *)aux;

    if (pokemon_buscar_ataque(pokemon, ataque) != NULL) {
        return false;
    }
    return true;
}

jugada_t adversario_proxima_jugada(adversario_t *adversario)
{
    srand((unsigned int)time(NULL));
    size_t tamanio = abb_tamanio(adversario->ataques);
    int pokemon_random = rand() % 3;
    int ataque_random = rand() % (int)tamanio;
    
    // Ensure ataque_random is within bounds
    if (ataque_random >= sizeof(adversario->ataques_totales) / sizeof(adversario->ataques_totales[0])) {
        ataque_random = sizeof(adversario->ataques_totales) / sizeof(adversario->ataques_totales[0]) - 1;
    }
    
    pokemon_t *pokemon = adversario->pokemones[pokemon_random];
    char *ataque = adversario->ataques_totales[ataque_random];

    while (!ataque_pokemon(ataque, pokemon) || abb_buscar(adversario->ataques, ataque) == NULL) {
        ataque_random = rand() % (int)tamanio;
        ataque = adversario->ataques_totales[ataque_random];
    }		

    jugada_t j = {"", ""};
    strcpy(j.pokemon, pokemon_nombre(pokemon));
	strcpy(j.ataque, ataque);

    abb_quitar(adversario->ataques, ataque);

    printf("Todo ok\n");

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

	lista_destruir(adversario->lista_pokemones);
	abb_destruir(adversario->ataques);

	for (int i = 0; i < 9; i++) {
        free(adversario->ataques_totales[i]);
    }

    for (int i = 0; i < 3; i++) {
        free(adversario->pokemones[i]);
    }

	free(adversario);
}
