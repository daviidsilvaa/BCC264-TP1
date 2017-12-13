
#define __USE_GNU 1
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <semaphore.h>

using namespace std;

vector<int> p;

sem_t pos_vazia;
sem_t pos_ocupada;

int inicio = 0, final = 0;
int i_pedido = -1;
int i_cliente;

void* cliente(void *v) {
    int i;

    int i_cli = i_cliente;

    for (i = 0; i < (random() % 5); i++) {
        sleep(random() % 2);  /* Permite que a outra thread execute */
        sem_wait(&pos_vazia);
        i_pedido += 1;

        p.push_back(i_pedido);
        printf("Cliente %d, pedido = %d\n", i_cli, p.back());

        sem_post(&pos_ocupada);
    }

    return NULL;
}

void* garcom(void *v) {
    int i;

    sleep(2);  /* Permite que a outra thread execute */

    while(p.begin() != p.end()){
        sem_wait(&pos_ocupada);

        printf("\tGarçom, pedido = %d\n", p.front());
        p.erase(p.begin());

        sem_post(&pos_vazia);
        sleep(random() % 3);  /* Permite que a outra thread execute */
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t thr_cliente[atoi(argv[1])], thr_garcom;

    sem_init(&pos_vazia, 0, atoi(argv[1]));
    sem_init(&pos_ocupada, 0, 0);

    pthread_create(&thr_garcom, NULL, garcom, NULL);
    for(int i = 0; i < atoi(argv[1]); i++){
        i_cliente = i + 1;
        pthread_create(&thr_cliente[i], NULL, cliente, NULL);
        sleep(0.5);
    }

    for(int i = 0; i < atoi(argv[1]); i++){
        pthread_join(thr_cliente[i], NULL);
    }
    pthread_join(thr_garcom, NULL);

    sem_destroy(&pos_vazia);
    sem_destroy(&pos_ocupada);

    return 0;
}
