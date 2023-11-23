#include "juego.h"
#include "ataque.h"
#include "lista.h"
#include "pokemon.h"
#include "tipo.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct info_jugador {
	int puntaje;
	struct ataque ataques_usados[9];
} info_jugador_t;

struct juego {
	lista_t *lista_pokemones;
	informacion_pokemon_t *info_pokemon;
	info_jugador_t *info_jugador1;
	info_jugador_t *info_jugador2;
	int turno;
};

juego_t *juego_crear()
{
	juego_t *juego = calloc(1, sizeof(juego_t));

	if (juego == NULL)
		return NULL;

	juego->info_jugador1 = calloc(1, sizeof(info_jugador_t));
	juego->info_jugador2 = calloc(1, sizeof(info_jugador_t));

	if (juego->info_jugador1 == NULL) {
		free(juego->info_jugador2);
		free(juego);
		return NULL;
	}

	if (juego->info_jugador2 == NULL) {
		free(juego->info_jugador1);
		free(juego);
		return NULL;
	}

	return juego;
}

JUEGO_ESTADO juego_cargar_pokemon(juego_t *juego, char *archivo)
{
	juego->info_pokemon = pokemon_cargar_archivo(archivo);

	if (juego->info_pokemon == NULL) {
		return ERROR_GENERAL;
	}

	int cantidad_pokemon = pokemon_cantidad(juego->info_pokemon);

	if (cantidad_pokemon < 6) {
		return POKEMON_INSUFICIENTES;
	}

	return TODO_OK;
}

void insertar_pokemon(pokemon_t *pokemon, void *aux)
{
	lista_insertar((lista_t *)aux, pokemon);
}

lista_t *juego_listar_pokemon(juego_t *juego)
{
	if (juego == NULL) {
		return NULL;
	}

	if (pokemon_cantidad(juego->info_pokemon) < 6) {
		pokemon_destruir_todo(juego->info_pokemon);
		return NULL;
	}

	juego->lista_pokemones = lista_crear();

	if (juego->lista_pokemones == NULL)
		return NULL;

	con_cada_pokemon(juego->info_pokemon, insertar_pokemon,
			 juego->lista_pokemones);

	return juego->lista_pokemones;
}

JUEGO_ESTADO juego_seleccionar_pokemon(juego_t *juego, JUGADOR jugador,
				       const char *nombre1, const char *nombre2,
				       const char *nombre3)
{
	if (juego == NULL || nombre1 == NULL || nombre2 == NULL ||
	    nombre3 == NULL)
		return ERROR_GENERAL;

	pokemon_t *pokemon1 = pokemon_buscar(juego->info_pokemon, nombre1);
	pokemon_t *pokemon2 = pokemon_buscar(juego->info_pokemon, nombre2);
	pokemon_t *pokemon3 = pokemon_buscar(juego->info_pokemon, nombre3);

	if (pokemon1 == NULL || pokemon2 == NULL || pokemon3 == NULL)
		return POKEMON_INEXISTENTE;

	if (strcmp(nombre1, nombre2) == 0 || strcmp(nombre1, nombre3) == 0 ||
	    strcmp(nombre2, nombre3) == 0)
		return POKEMON_REPETIDO;

	return TODO_OK;
}

RESULTADO_ATAQUE efectividad_ataque(enum TIPO tipo_ataque,
				    enum TIPO tipo_pokemon)
{
	if (tipo_ataque == tipo_pokemon || tipo_ataque == NORMAL)
		return ATAQUE_REGULAR;
	switch (tipo_ataque) {
	case FUEGO:
		if (tipo_pokemon == PLANTA)
			return ATAQUE_EFECTIVO;
		else if (tipo_pokemon == AGUA)
			return ATAQUE_INEFECTIVO;
	case PLANTA:
		if (tipo_pokemon == ROCA)
			return ATAQUE_EFECTIVO;
		else if (tipo_pokemon == FUEGO)
			return ATAQUE_INEFECTIVO;
	case ROCA:
		if (tipo_pokemon == ELECTRICO)
			return ATAQUE_EFECTIVO;
		else if (tipo_pokemon == PLANTA)
			return ATAQUE_INEFECTIVO;
	case ELECTRICO:
		if (tipo_pokemon == AGUA)
			return ATAQUE_EFECTIVO;
		else if (tipo_pokemon == ROCA)
			return ATAQUE_INEFECTIVO;
	case AGUA:
		if (tipo_pokemon == FUEGO)
			return ATAQUE_EFECTIVO;
		else if (tipo_pokemon == ELECTRICO)
			return ATAQUE_INEFECTIVO;
	default:
		return -1;
	}
}

int calcular_puntaje(RESULTADO_ATAQUE resultado, unsigned int poder)
{
	int poder_final = (int)poder;

	switch (resultado) {
	case ATAQUE_REGULAR:
		return poder_final;
	case ATAQUE_EFECTIVO:
		return poder_final * 3;
	case ATAQUE_INEFECTIVO:
		return (int)ceil(poder_final / 2);
	default:
		return 0;
	}
}

bool verificar_ataque(const struct ataque *ataque, info_jugador_t *info_jugador,
		      juego_t *juego)
{
	for (int i = 0; i < juego->turno; i++) {
		if (strcmp(info_jugador->ataques_usados[i].nombre,
			   ataque->nombre) == 0)
			return false;
	}
	return true;
}

resultado_jugada_t juego_jugar_turno(juego_t *juego, jugada_t jugada_jugador1,
				     jugada_t jugada_jugador2)
{
	resultado_jugada_t resultado;
	resultado.jugador1 = ATAQUE_ERROR;
	resultado.jugador2 = ATAQUE_ERROR;

	if (juego == NULL)
		return resultado;

	pokemon_t *pokemon_jugador1 =
		pokemon_buscar(juego->info_pokemon, jugada_jugador1.pokemon);

	pokemon_t *pokemon_jugador2 =
		pokemon_buscar(juego->info_pokemon, jugada_jugador2.pokemon);

	if (pokemon_jugador1 == NULL || pokemon_jugador2 == NULL)
		return resultado;

	const struct ataque *ataque_jugador1 =
		pokemon_buscar_ataque(pokemon_jugador1, jugada_jugador1.ataque);

	const struct ataque *ataque_jugador2 =
		pokemon_buscar_ataque(pokemon_jugador2, jugada_jugador2.ataque);

	if (ataque_jugador1 == NULL || ataque_jugador2 == NULL ||
	    verificar_ataque(ataque_jugador1, juego->info_jugador1, juego) ==
		    false ||
	    verificar_ataque(ataque_jugador2, juego->info_jugador2, juego) ==
		    false)
		return resultado;

	resultado.jugador1 = efectividad_ataque(ataque_jugador1->tipo,
						pokemon_tipo(pokemon_jugador2));

	resultado.jugador2 = efectividad_ataque(ataque_jugador2->tipo,
						pokemon_tipo(pokemon_jugador1));

	juego->info_jugador1->puntaje +=
		calcular_puntaje(resultado.jugador1, ataque_jugador1->poder);

	juego->info_jugador2->puntaje +=
		calcular_puntaje(resultado.jugador2, ataque_jugador2->poder);

	juego->info_jugador1->ataques_usados[juego->turno] = *ataque_jugador1;

	juego->info_jugador2->ataques_usados[juego->turno] = *ataque_jugador2;

	juego->turno++;

	return resultado;
}

int juego_obtener_puntaje(juego_t *juego, JUGADOR jugador)
{
	if (juego == NULL)
		return 0;

	if (jugador == JUGADOR1)
		return juego->info_jugador1->puntaje;

	return juego->info_jugador2->puntaje;
}

bool juego_finalizado(juego_t *juego)
{
	return false;
}

void juego_destruir(juego_t *juego)
{
	if (juego == NULL)
		return;

	if (juego->lista_pokemones != NULL)
		lista_destruir(juego->lista_pokemones);

	if (juego->info_pokemon != NULL)
		pokemon_destruir_todo(juego->info_pokemon);

	free(juego->info_jugador1);
	free(juego->info_jugador2);

	free(juego);
}
