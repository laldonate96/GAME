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

bool jugador_seleccionar_pokemon(jugador_t *jugador, char **nombre1,
				    char **nombre2, char **nombre3)
{
    for (int i = 0; i < 3; i++)
    {
        char linea[20]; 
        printf("Elija un pokemon para agregar a tu mazo: ");
        fgets(linea, sizeof(linea), stdin);
        linea[strcspn(linea, "\n")] = '\0';
        
        if (i == 0) {
            *nombre1 = malloc(sizeof(char) * 20);
            strcpy(*nombre1, linea);
        } else if (i == 1) {
            *nombre2 = malloc(sizeof(char) * 20);
            strcpy(*nombre2, linea);
        } else if (i == 2) {
            *nombre3 = malloc(sizeof(char) * 20);
            strcpy(*nombre3, linea);
        }
    }
    return true;
}

bool jugador_pokemon_existe(jugador_t *jugador, char *pokemon)
{
    for (int i = 0; i < 3; i++)
    {
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
    // while (!jugador_pokemon_existe(jugador, nombre))
    // {
    //     printf("El pokemon ingresado no existe o ya no tiene ataques disponibles, ingrese otro: ");
    //     fgets(linea, sizeof(linea), stdin);
    //     linea[strcspn(linea, "\n")] = '\0';
    //     strcpy(nombre, linea);
    // }

    printf("Ingrese el nombre del ataque: ");
    fgets(linea, sizeof(linea), stdin);
    linea[strcspn(linea, "\n")] = '\0';
    strcpy(ataque, linea);
    // while (!jugador_ataque_existe(jugador, ataque))
    // {
    //     printf("El ataque ingresado no existe o ya fue utilizado, ingrese otro: ");
    //     fgets(linea, sizeof(linea), stdin);
    //     linea[strcspn(linea, "\n")] = '\0';
    //     strcpy(ataque, linea);
    // }

    jugada_t jugada = {"", ""};
    strcpy(jugada.pokemon, nombre);
    strcpy(jugada.ataque, ataque);
    
    return jugada;
}

void jugador_destruir(jugador_t *jugador)
{
    
}