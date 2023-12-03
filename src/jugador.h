#ifndef JUGADOR_H_
#define JUGADOR_H_

#include "lista.h"
#include "juego.h"
#include "pokemon.h"
#include <stdlib.h>

typedef struct jugador jugador_t;

/**
 * Crea un jugador asignandole un listado de pokemon_t que puede utilizar
 * durante el juego.
 *
 * El jugador NO deberá modificar la lista ni liberarla.
 *
 * Devuelve NULL en caso de error.
 */
jugador_t *jugador_crear(lista_t *);

/**
 * El jugador selecciona los pokemon para utilizar durante el juego. Modifica
 * los 3 punteros pasados para hacerlos apuntar a los nombres de los 3 pokemon
 * que quiere seleccionar.
 *
 * Los primeros 2 pokemon pertenecen al jugador, el tercer pokemon es para el
 * adversario.
 *
 * Devuelve true si pudo seleccionar o false en caso de error,
 */
bool jugador_seleccionar_pokemon(jugador_t *, char **nombre1, char **nombre2,
				 char **nombre3);

/**
 * Pide al jugador que ingrese por consola el pokemon y ataque para la
 * siguiente ronda y valida los datos.
 */
jugada_t jugador_pedir_nombre_y_ataque(jugador_t *);

/**
 * Asigna el tercer pokemon del jugador, en base a la elección del adversario.
 */
void jugador_tercer_pokemon(jugador_t *, char *nombre3);

/**
 * Destruye el jugador y libera la memoria reservada.
 */
void jugador_destruir(jugador_t *);

#endif // JUGADOR_H_