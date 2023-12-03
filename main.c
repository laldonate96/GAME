#include "src/abb.h"
#include "src/adversario.h"
#include "src/jugador.h"
#include "src/ataque.h"
#include "src/juego.h"
#include "src/lista.h"
#include "src/pokemon.h"
#include "src/tipo.h"
#include <stdio.h>
#include <string.h>

/**
 * Este main debe ser modificado para que el usuario pueda jugar el juego. Las
 * instrucciones existentes son solamente a modo ilustrativo del funcionamiento
 * muy alto nivel del juego.
 *
 * Las interacciones deben realizarse por entrada/salida estandar y estar
 * validadas.
 *
 * Se aconseja en todo momento mostrar información relevante para el jugador
 * (por ejemplo, mostrar puntaje actual y movimientos disponibles) para hacer
 * que el juego sea facil de utilizar.
 */

#define ARCHIVO_OK_1 "ejemplos/correcto.txt"

char *pedir_archivo()
{
	char *archivo = calloc(1, sizeof(char) * 255);
	printf("------------------------------------------------------------------------------------------------------------------\n");
	printf("Ingrese el nombre del archivo de pokemones: ");
	scanf("%254s", archivo);
	while (strcmp(archivo, ARCHIVO_OK_1) != 0) {
		printf("El archivo ingresado no es válido, por favor ingrese otro: ");
		scanf("%254s", archivo);
	}
	printf("------------------------------------------------------------------------------------------------------------------\n");

	int c;
	while ((c = getchar()) != '\n' && c != EOF) {
	};

	return archivo;
}

void mostrar_pokemon_disponibles(juego_t *juego)
{
	lista_t *lista_pokemon = juego_listar_pokemon(juego);
	printf("Los pokemones disponibles son:\n");
	for (size_t i = 0; i < lista_tamanio(lista_pokemon); i++) {
		pokemon_t *pokemon =
			lista_elemento_en_posicion(lista_pokemon, i);
		printf("%s\n", pokemon_nombre(pokemon));
	}
	printf("------------------------------------------------------------------------------------------------------------------\n");
}

int main(int argc, char *argv[])
{
	juego_t *juego = juego_crear();

	// Pide al usuario un nombre de archivo de pokemones
	char *archivo = pedir_archivo();

	// Carga los pokemon
	juego_cargar_pokemon(juego, archivo);

	// Crea un jugador que será utilizado como jugador 1
	jugador_t *jugador = jugador_crear(juego_listar_pokemon(juego));

	// Crea un adversario que será utilizado como jugador 2
	adversario_t *adversario =
		adversario_crear(juego_listar_pokemon(juego));

	// Mostrar el listado de pokemones por consola para que el usuario sepa las
	// opciones que tiene
	mostrar_pokemon_disponibles(juego);

	// Pedirle al jugador por consola que ingrese los 3 nombres de pokemon que
	// quiere utilizar
	char *eleccionJugador1, *eleccionJugador2, *eleccionJugador3;
	jugador_seleccionar_pokemon(jugador, &eleccionJugador1,
				    &eleccionJugador2, &eleccionJugador3);

	// pedirle al adversario que indique los 3 pokemon que quiere usar
	char *eleccionAdversario1, *eleccionAdversario2, *eleccionAdversario3;
	adversario_seleccionar_pokemon(adversario, &eleccionAdversario1,
				       &eleccionAdversario2,
				       &eleccionAdversario3);

	// Seleccionar los pokemon de los jugadores
	juego_seleccionar_pokemon(juego, JUGADOR1, eleccionJugador1,
				  eleccionJugador2, eleccionAdversario3);
	juego_seleccionar_pokemon(juego, JUGADOR2, eleccionAdversario1,
				  eleccionAdversario2, eleccionAdversario3);

	// informarle al adversario cuales son los pokemon del jugador
	adversario_pokemon_seleccionado(adversario, eleccionJugador1,
					eleccionJugador2, eleccionJugador3);

	// asignar el tercer pokemon del jugador
	jugador_tercer_pokemon(jugador, eleccionAdversario3);

	// informarle al jugador cuales son los pokemon del adversario
	printf("Los pokemones del jugador son: %s, %s y %s\n", eleccionJugador1,
	       eleccionJugador2, eleccionAdversario3);

	free(eleccionAdversario3);

	while (!juego_finalizado(juego)) {
		resultado_jugada_t resultado_ronda;

		// Pide al jugador que ingrese por consola el pokemon y ataque para la
		// siguiente ronda
		jugada_t jugada_jugador =
			jugador_pedir_nombre_y_ataque(jugador);

		// Pide al adversario que informe el pokemon y ataque para la siguiente
		// ronda
		jugada_t jugada_adversario =
			adversario_proxima_jugada(adversario);

		// jugar la ronda y después comprobar que esté todo ok, si no, volver a
		// pedir la jugada del jugador
		resultado_ronda = juego_jugar_turno(juego, jugada_jugador,
						    jugada_adversario);

		if (resultado_ronda.jugador1 == ATAQUE_ERROR) {
			printf("El ataque no corresponde al pokemon o ya fue utilizado!\n");
		}

		// Si se pudo jugar el turno, le informo al adversario la jugada realizada
		// por el jugador
		if (resultado_ronda.jugador1 != ATAQUE_ERROR &&
		    resultado_ronda.jugador2 != ATAQUE_ERROR) {
			adversario_informar_jugada(adversario, jugada_jugador);
			printf("El adversario eligió el ataque %s del pokemon %s\n",
			       jugada_adversario.ataque,
			       jugada_adversario.pokemon);
		}
	}

	jugador_destruir(jugador);
	adversario_destruir(adversario);
	juego_destruir(juego);
}
