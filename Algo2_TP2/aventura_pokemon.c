#include "aventura_pokemon.h"
#include "m_heap.h"
#include "m_lista.h"

#define FORMATO_PRELECTURA "%[^;];"
#define FORMATO_LECTURA_GIMANSIO "%100[^;];%i;%i\n"
#define FORMATO_LECTURA_ENTRENADOR "%50[^\n]\n"
#define FORMATO_LECTURA_POKEMON "%100[^;];%i;%i;%i\n"

#define Gimnasio 1
#define Lider 2
#define Entrenador 3
#define Pokemon 4

#define OK 0
#define ERROR -1


/* ++++++++++++++++++++++++++++++++++++++++++++++++++ FUNCIONES DE LECTURA ++++++++++++++++++++++++++++++++++++++++++++++++++ */

int leer_comienzo_linea (FILE* info_gimnasio){
    char comienzo_linea;
    int leer_comienzo_linea = fscanf(info_gimnasio, FORMATO_PRELECTURA, &comienzo_linea);
    if(leer_comienzo_linea != 1) return ERROR;

    if (comienzo_linea == 'G') return Gimnasio;
    else if (comienzo_linea == 'L') return Lider;
    else if (comienzo_linea == 'E') return Entrenador;
    else if (comienzo_linea == 'P') return Pokemon;
    else return ERROR;
}

pokemon_t leer_pokemon(FILE* info_gimnasio){
    pokemon_t pokemon;
    int leer_linea = fscanf(info_gimnasio, FORMATO_LECTURA_POKEMON, pokemon.especie, &(pokemon).velocidad, &(pokemon).defensa, &(pokemon).ataque);
    if(leer_linea != 4) pokemon.especie == NULL;
    
    return pokemon;
}

entrenador_t leer_entrenador(FILE* info_gimnasio){

    entrenador_t entrenador;
    int leer_linea = fscanf(info_gimnasio, FORMATO_LECTURA_ENTRENADOR, entrenador.nombre);
    if(leer_linea != 1) entrenador.nombre == NULL;

    return entrenador;
}

gimnasio_t leer_gimnasio(FILE* info_gimnasio){

    gimnasio_t gimnasio;
    int leer_linea = fscanf(info_gimnasio, FORMATO_LECTURA_GIMANSIO, gimnasio.nombre, &(gimnasio).dificultad, &(gimnasio).id_puntero_funcion);
    if(leer_linea != 3) gimnasio.nombre == NULL;

    return gimnasio;
}
/* ++++++++++++++++++++++++++++++++++++++++++++++++++ FUNCIONES CREACION DE ESTRUCTURAS ++++++++++++++++++++++++++++++++++++++++++++++++++ */

pokemon_t* crear_pokemon(FILE* info_gimnasio){
    if(!info_gimnasio) return NULL;

    pokemon_t pokemon_leido = leer_pokemon(info_gimnasio);
    if(!pokemon_leido.especie) return NULL;

    pokemon_t* pokemon = calloc(1, sizeof(pokemon_t));
    if(!pokemon) return NULL;

    strcpy(pokemon->especie, pokemon_leido.especie);
    pokemon->ataque = pokemon_leido.ataque;
    pokemon->defensa = pokemon_leido.defensa;
    pokemon->velocidad = pokemon_leido.velocidad;
    return pokemon;
}

entrenador_t* crear_entrenador(FILE* info_gimnasio){
    if(!info_gimnasio) return NULL;

    entrenador_t entrenador_leido = leer_entrenador(info_gimnasio);
    if(!entrenador_leido.nombre) return NULL;

    entrenador_t* entrenador = calloc(1, sizeof(entrenador_t));
    if(!entrenador) return NULL;

    strcpy(entrenador->nombre, entrenador_leido.nombre);
    return entrenador;
}

gimnasio_t* crear_gimnasio(FILE* info_gimnasio){
    if(!info_gimnasio) return NULL;

    gimnasio_t gimnasio_leido = leer_gimnasio(info_gimnasio);
    if(!gimnasio_leido.nombre) return NULL;

    gimnasio_t* gimnasio = calloc(1, sizeof(gimnasio_t));
    if(!gimnasio) return NULL;

    strcpy(gimnasio->nombre, gimnasio_leido.nombre);
    gimnasio->dificultad = gimnasio_leido.dificultad;
    gimnasio->id_puntero_funcion = gimnasio_leido.id_puntero_funcion;

    return gimnasio;
}

/* ++++++++++++++++++++++++++++++++++++++++++++++++++ FUNCIONES DESTRUCCION DE ESTRUCTURAS ++++++++++++++++++++++++++++++++++++++++++++++++++ */

void destruir_pokemon(pokemon_t* pokemon){
    if(pokemon) free(pokemon);
}

void destruir_entrenador(entrenador_t* entrenador){
    if(entrenador->v_pokemones)
        lista_destruir(entrenador->v_pokemones);
    if(entrenador) free(entrenador);
}
void destruir_gimnasio(gimnasio_t* gimnasio, heap_destructor_elemento destructor){
    if(gimnasio) destructor((void*)gimnasio);
    /*
    //Libero todos los entrenadores, su lista_t de pokemones y el pokemon en si.
    for(int i = 0; i < gimnasio->cont_entrenadores; i++){ 
        entrenador_t* ultimo_entrenador = lista_ultimo(gimnasio->v_entrenadores);
        destruir_entrenador(ultimo_entrenador);
        int n = lista_borrar(gimnasio->v_entrenadores);//Hacer algo con el n???
    }

    if(gimnasio->v_entrenadores) lista_destruir(gimnasio->v_entrenadores); //Libero lista_t de entrenadores
    if(gimnasio) free(gimnasio);
    */
}

/* ++++++++++++++++++++++++++++++++++++++++++++++++++ FUNCIONES PRINCIPALES ++++++++++++++++++++++++++++++++++++++++++++++++++ */

void lectura_cargado_archivo(FILE* info_gimnasio, heap_t* heap, gimnasio_t* gimnasio, entrenador_t* entrenador, int* bandera){

    int comienzo_linea = leer_comienzo_linea(info_gimnasio);
    if(comienzo_linea == ERROR) {
        (*bandera) = ERROR;
        if(gimnasio) destruir_gimnasio(gimnasio, heap->destructor);
        return ;
    }

    int bandera_insercion_lista = -1;
    gimnasio_t* nuevo_gimnasio;
    entrenador_t* nuevo_entrenador;
    pokemon_t* nuevo_pokemon;

    switch (comienzo_linea){
        case Gimnasio:
            if(gimnasio && ((entrenador_t*)gimnasio->v_entrenadores->nodo_fin->elemento)->cantidad_pokemones > 0) //Si ya existia un gimnasio y su ultimo entrenador tiene al menos 1 pokemon 
                heap_insertar_nodo(heap, gimnasio);
            else if(gimnasio && !((entrenador_t*)gimnasio->v_entrenadores->nodo_fin->elemento)->cantidad_pokemones > 0){
                destruir_gimnasio(gimnasio, heap->destructor);
                (*bandera) = ERROR;
                return;
            }

            nuevo_gimnasio = crear_gimnasio(info_gimnasio);
            if(!nuevo_gimnasio){
                (*bandera) = ERROR;
                return;
            }
            lectura_cargado_archivo(info_gimnasio, heap, nuevo_gimnasio, NULL, bandera);
            break;
        case Lider:
            if(gimnasio->v_entrenadores->nodo_inicio){
                (*bandera) = ERROR;
                return;
            }
            nuevo_entrenador = crear_entrenador(info_gimnasio);
            if(!nuevo_entrenador){
                (*bandera) = ERROR;
                return;
            }
            bandera_insercion_lista = lista_insertar(gimnasio->v_entrenadores, nuevo_entrenador);
            if(bandera_insercion_lista != 0){
                destruir_entrenador(nuevo_entrenador);
                (*bandera) = ERROR;
                return;
            }
            lectura_cargado_archivo(info_gimnasio, heap, gimnasio, nuevo_entrenador, bandera);
            break;
        case Entrenador:
            if(!gimnasio->v_entrenadores->nodo_inicio){ //Si no hay Lider y/o entrenadores previos
                (*bandera) = ERROR;
                return;
            }
            nuevo_entrenador = crear_entrenador(info_gimnasio);
            if(!nuevo_entrenador){
                (*bandera) = ERROR;
                return;
            }
            bandera_insercion_lista = lista_insertar(gimnasio->v_entrenadores, nuevo_entrenador);
            if(bandera_insercion_lista != 0){
                destruir_entrenador(nuevo_entrenador);
                (*bandera) = ERROR;
                return;
            }
            lectura_cargado_archivo(info_gimnasio, heap, gimnasio, nuevo_entrenador, bandera);
            break;
        case Pokemon:
            if(!entrenador){
                (*bandera) = ERROR;
                return;
            }
            nuevo_pokemon = crear_pokemon(info_gimnasio);
            bandera_insercion_lista = lista_insertar( ((entrenador_t*)gimnasio->v_entrenadores->nodo_fin->elemento)->v_pokemones, nuevo_pokemon);
            if(bandera_insercion_lista != 0){
                destruir_pokemon(nuevo_pokemon);
                (*bandera) = ERROR;
                return;
            }
            break;
        default:
            destruir_gimnasio(gimnasio, heap->destructor);
            (*bandera) = ERROR;
            return;
    }
    
}

int cargar_archivo(const char* ruta_archivo, heap_t* heap){
    if(!ruta_archivo || !heap) return ERROR;
    int bandera = OK; //creo bandera, retorna 0 si se pudo cargar el gimnasio, -1 si no lo pudo cargar

    FILE* info_gimnasio = fopen(ruta_archivo,"r");
    if(!info_gimnasio) return bandera = ERROR;
   

    lectura_cargado_archivo(info_gimnasio, heap, NULL, NULL, &bandera);

    fclose(info_gimnasio);
    return bandera;
}