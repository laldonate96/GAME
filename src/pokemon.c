#include "pokemon.h"
#include "ataque.h"
#include "tipo.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ATAQUES 3
#define NOMBRE 20
#define MAX_LINEA 255
#define READ "r"

struct pokemon {
	char nombre[NOMBRE];
	enum TIPO tipo;
	struct ataque *ataques[MAX_ATAQUES];
};

struct info_pokemon {
	pokemon_t **pokemones;
	int cantidad;
	size_t capacidad;
};

/**
 * Determina el tipo de pokemon a partir de un caracter y lo devuelve.
 *
 * En caso de que el caracter no sea válido, devuelve -1.
 */
enum TIPO tipo_pokemon(const char tipo)
{
	switch (tipo) {
	case 'N':
		return NORMAL;
	case 'F':
		return FUEGO;
	case 'A':
		return AGUA;
	case 'P':
		return PLANTA;
	case 'E':
		return ELECTRICO;
	case 'R':
		return ROCA;
	default:
		return -1;
	}
}

/**
 * En caso de que los ataques no sean válidos, libera la memoria y
 * devuelve NULL.
 */

informacion_pokemon_t *ataques_invalidos(informacion_pokemon_t *ip,
					 FILE *archivo)
{
	fclose(archivo);
	for (int k = 0; k < 3; k++) {
		free(ip->pokemones[ip->cantidad]->ataques[k]);
	}
	free(ip->pokemones[ip->cantidad]);
	free(ip->pokemones);
	free(ip);
	return NULL;
}

/**
 * En caso de que el tipo no sea válido, libera la memoria y
 * devuelve el vector con la info acumulada hasta dicho momento.
 */
informacion_pokemon_t *tipo_invalido(informacion_pokemon_t *ip, FILE *archivo)
{
	fclose(archivo);
	for (int k = 0; k < 3; k++) {
		free(ip->pokemones[ip->cantidad]->ataques[k]);
	}
	free(ip->pokemones[ip->cantidad]);
	return ip;
}

/**
 * Intercambia la posición de dos pokemones.
 */
void cambiar_posicion(pokemon_t *xp, pokemon_t *yp)
{
	pokemon_t temp = *xp;
	*xp = *yp;
	*yp = temp;
}

/**
 * Ordena los pokemones por orden alfabético.
 */
void bubble_sort(pokemon_t **pokemones, int cantidad)
{
	int i, j;
	int es_mayor;
	bool cambio;
	for (i = 0; i < cantidad - 1; i++) {
		cambio = false;
		for (j = 0; j < cantidad - i - 1; j++) {
			es_mayor = strcmp(pokemones[j]->nombre,
					  pokemones[j + 1]->nombre);
			if (es_mayor > 0) {
				cambiar_posicion(pokemones[j],
						 pokemones[j + 1]);
				cambio = true;
			}
		}

		if (cambio == false)
			break;
	}
}

/**
 * Realiza el realloc en caso de ser posible.
 */
bool funcion_realloc(informacion_pokemon_t *info, FILE *archivo)
{
	if (info->cantidad >= info->capacidad) {
		info->capacidad *= 2;
		pokemon_t **aux_pokemones = realloc(
			info->pokemones, info->capacidad * sizeof(pokemon_t));

		if (info->pokemones == NULL) {
			fclose(archivo);
			free(info->pokemones);
			return info;
		}

		info->pokemones = aux_pokemones;
	}
	return false;
}

/**
 * Inicializa un pokemon en caso de ser posible.
 */

bool inicializar_pokemon(informacion_pokemon_t *info, FILE *archivo)
{
	info->pokemones[info->cantidad] = malloc(sizeof(pokemon_t));

	if (info->pokemones[info->cantidad] == NULL) {
		fclose(archivo);
		free(info->pokemones);
		free(info);
		return NULL;
	}
	return false;
}

/**
 * Inicializa un ataque en caso de ser posible.
 */

bool inicializar_ataque(informacion_pokemon_t *info, int i, FILE *archivo)
{
	info->pokemones[info->cantidad]->ataques[i] =
		malloc(sizeof(struct ataque));

	if (info->pokemones[info->cantidad]->ataques[i] == NULL) {
		fclose(archivo);
		free(info->pokemones);
		free(info);
		return NULL;
	}

	return false;
}

/**
 * Obtiene los ataques de un pokemon en caso de ser posible.
 */
int obtener_ataques(char linea[MAX_LINEA], char aux_tipo_ataques[][NOMBRE],
		    informacion_pokemon_t *info, FILE *archivo, int leidos,
		    int i)
{
	if (fgets(linea, MAX_LINEA, archivo) != NULL) {
		leidos = sscanf(
			linea, "%[^;];%[^;];%u",
			info->pokemones[info->cantidad]->ataques[i]->nombre,
			aux_tipo_ataques[i],
			&info->pokemones[info->cantidad]->ataques[i]->poder);
	}
	return leidos;
}

informacion_pokemon_t *pokemon_cargar_archivo(const char *path)
{
	if (path == NULL) {
		return NULL;
	}

	FILE *archivo = fopen(path, READ);

	if (archivo == NULL) {
		return NULL;
	}

	struct info_pokemon *info = malloc(sizeof(struct info_pokemon));

	if (info == NULL) {
		fclose(archivo);
		return NULL;
	}

	char aux_tipo_pokemon[NOMBRE];

	char aux_tipo_ataques[MAX_ATAQUES][NOMBRE];

	char linea[MAX_LINEA];

	int leidos;

	info->cantidad = 0;

	info->capacidad = 1;

	info->pokemones = malloc(info->capacidad * sizeof(pokemon_t));

	if (info->pokemones == NULL) {
		fclose(archivo);
		free(info);
		return NULL;
	}

	while (fgets(linea, sizeof(linea), archivo) != NULL) {
		funcion_realloc(info, archivo);

		inicializar_pokemon(info, archivo);

		leidos = sscanf(linea, "%[^;];%[^;]",
				info->pokemones[info->cantidad]->nombre,
				aux_tipo_pokemon);

		if (leidos != 2) {
			fclose(archivo);
			return info;
		}

		info->pokemones[info->cantidad]->tipo =
			tipo_pokemon(aux_tipo_pokemon[0]);

		for (int i = 0; i < MAX_ATAQUES; i++) {
			inicializar_ataque(info, i, archivo);

			leidos = obtener_ataques(linea, aux_tipo_ataques, info,
						 archivo, leidos, i);

			if (leidos != MAX_ATAQUES)
				return ataques_invalidos(info, archivo);

			else if (tipo_pokemon(aux_tipo_ataques[i][0]) == -1)
				return tipo_invalido(info, archivo);

			else
				info->pokemones[info->cantidad]
					->ataques[i]
					->tipo =
					tipo_pokemon(aux_tipo_ataques[i][0]);
		}

		info->cantidad++;
	}

	fclose(archivo);

	bubble_sort(info->pokemones, info->cantidad);

	return info;
}

pokemon_t *pokemon_buscar(informacion_pokemon_t *ip, const char *nombre)
{
	if (ip == NULL || nombre == NULL)
		return NULL;

	for (int i = 0; i < ip->cantidad; i++) {
		if (strcmp(ip->pokemones[i]->nombre, nombre) == 0) {
			return ip->pokemones[i];
		}
	}

	return NULL;
}

int pokemon_cantidad(informacion_pokemon_t *ip)
{
	if (ip == NULL)
		return 0;

	return ip->cantidad;
}

const char *pokemon_nombre(pokemon_t *pokemon)
{
	if (pokemon == NULL)
		return NULL;

	return pokemon->nombre;
}

enum TIPO pokemon_tipo(pokemon_t *pokemon)
{
	if (pokemon == NULL)
		return NORMAL;

	return pokemon->tipo;
}

const struct ataque *pokemon_buscar_ataque(pokemon_t *pokemon,
					   const char *nombre)
{
	for (int i = 0; i < 3; i++) {
		if (strcmp(pokemon->ataques[i]->nombre, nombre) == 0) {
			return pokemon->ataques[i];
		}
	}
	return NULL;
}

int con_cada_pokemon(informacion_pokemon_t *ip, void (*f)(pokemon_t *, void *),
		     void *aux)
{
	if (f == NULL)
		return 0;

	if (ip == NULL)
		return 0;

	int i;

	for (i = 0; i < ip->cantidad; i++) {
		f(ip->pokemones[i], aux);
	}

	return i;
}

int con_cada_ataque(pokemon_t *pokemon,
		    void (*f)(const struct ataque *, void *), void *aux)
{
	if (f == NULL)
		return 0;

	else if (pokemon == NULL)
		return 0;

	int i;

	for (i = 0; i < MAX_ATAQUES; i++) {
		f(pokemon->ataques[i], aux);
	}

	return i;
}

void pokemon_destruir_todo(informacion_pokemon_t *ip)
{
	if (ip == NULL)
		return;

	for (int i = 0; i < ip->cantidad; i++) {
		for (int j = 0; j < MAX_ATAQUES; j++) {
			free(ip->pokemones[i]->ataques[j]);
		}
		free(ip->pokemones[i]);
	}
	free(ip->pokemones);
	free(ip);
}
