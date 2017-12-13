#define __USE_GNU 1
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <semaphore.h>

using namespace std;

vector<int> pedidos;

sem_t pos_vazia;
sem_t pos_ocupada;

int inicio = 0, final = 0;
int i_pedido = -1;
int i_cliente;

// Rotina do cliente (produtor)
void* cliente(void *v) {
    int i;
    int i_cli = i_cliente;  // indica o indice do cliente para a rotina disparada

    // no maximo 5 pedidos
    for (i = 0; i < (random()%5); i++) {

        sleep(random() % 2);  // tempo para realização do pedido
        sem_wait(&pos_vazia);  // decrementa o valor do semáforo, se o semáforo está com valor zero, o processo é posto pra dormir
        i_pedido += 1;

        pedidos.push_back(i_pedido);
        printf("Cliente %d, pedido = %d.\n", i_cli, pedidos.back());

        sem_post(&pos_ocupada);  // se o semaforo estiver com valor zero e existir algum processo adormecido, um processo sera acordado
                                 // caso contrário, o valor do semaforo é incrementado
    }

    return NULL;
}

//Rotina do garcom (Consumidor)
void* garcom(void *v) {

    int i;

    sleep(2); //tempo para recepção do pedido

    while(pedidos.begin() != pedidos.end()){

        sem_wait(&pos_ocupada);

        printf("\tGarçom, pedido = %d.\n", pedidos.front());
        pedidos.erase(pedidos.begin()); // apagando primeiro elemento da fila de pedidos

        sem_post(&pos_vazia);

        sleep(random() % 3); // tempo para entrega do pedido
    }

    return NULL;
}

int main(int argc, char *argv[]) {

    //criando as threads

    pthread_t thr_cliente[atoi(argv[1])]; // criando vetor de threads do numero de clientes
    pthread_t thr_garcom; //criando thread garcom

    //Iniciando os semaforos -> sem_init(sem_t *sem, int pshared, unsigned int value);

    sem_init(&pos_vazia, 0,atoi(argv[1])); // iniciando semaforos de acordo com numero de clientes atoi(argv[1])
    sem_init(&pos_ocupada, 0, 0); //iniciando semaforo do garcom

    //criando thread do garcom
    pthread_create(&thr_garcom, NULL, garcom, NULL);

    printf("\n\t!Servico iniciado!\n\n");

    // criando as threads dos clientes
    for(int i = 0; i < atoi(argv[1]); i++){

        i_cliente = i + 1; // definindo para qual cliente a thread esta sendo criada

        //(pthread_t * thread , const pthread_attr_t * attr , void * (* start_routine ) (void *), void * arg );
        pthread_create(&thr_cliente[i], NULL, cliente, NULL);

        //sleep para que der tempo na troca do varivael que defini o cliente
        sleep(0.5);
    }

    //Roda enquanto  a thread nao for finalizada -> pthread_join

    for(int i = 0; i < atoi(argv[1]); i++){

        //pthread_join(pthread_t thread, void **value_ptr);
        pthread_join(thr_cliente[i], NULL);
    }

    //pthread_join(pthread_t thread, void **value_ptr);
    pthread_join(thr_garcom, NULL);
    printf("\n\t!Servico finalizado!\n");

    // destruindo os semaforos  ->  sem_destroy(sem_t *sem);
    sem_destroy(&pos_vazia);
    sem_destroy(&pos_ocupada);

    return 0;
}
