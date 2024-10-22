#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    pthread_mutex_t mutex;        // Мьютекс для защиты общей структуры

    pthread_cond_t readers;       // Условная переменная для читателей
    pthread_cond_t writers;       // Условная переменная для писателей

    uint64_t r_count;             // Счетчик читателей
    uint64_t w_lock_count;        // Счетчик количества потоков, ожидающих получения блокировки на запись
    uint64_t r_lock_count;        // Счетчик количества потоков, ожидающих получения блокировки на чтение

    uint16_t w_lock_flag;         // Флаг, показывающий, получил ли блокировку хотя бы один писатель в данный момент
} my_rwlock_t;

// инцилизация
void rwlock_init(my_rwlock_t* p) {
    pthread_mutex_init(&p->mutex, NULL);
    pthread_cond_init(&p->readers, NULL);
    pthread_cond_init(&p->writers, NULL);
    p->r_count = 0;
    p->w_lock_count = 0;
    p->r_lock_count = 0;
    p->w_lock_flag = 0;
}

// уничтожение
void rwlock_destroy(my_rwlock_t* p) {
    pthread_mutex_destroy(&p->mutex);
    pthread_cond_destroy(&p->readers);
    pthread_cond_destroy(&p->writers);
}


void rdlock(my_rwlock_t* p) {
    pthread_mutex_lock(&p->mutex);

    p->r_lock_count++;
    while (p->w_lock_flag || p->w_lock_count > 0) {
        pthread_cond_wait(&p->readers, &p->mutex);
    }
    p->r_lock_count--;
    p->r_count++;

    pthread_mutex_unlock(&p->mutex);
}

void wrlock(my_rwlock_t* p) {
    pthread_mutex_lock(&p->mutex);

    p->w_lock_count++;
    while (p->r_count > 0 || p->w_lock_flag) {
        pthread_cond_wait(&p->writers, &p->mutex);
    }
    p->w_lock_count--;
    p->w_lock_flag = 1;

    pthread_mutex_unlock(&p->mutex);
}


void unlock(my_rwlock_t* p) {
    pthread_mutex_lock(&p->mutex);

    if (p->w_lock_flag) {
        p->w_lock_flag = 0;
        if (p->w_lock_count > 0) {
            pthread_cond_signal(&p->writers);
        } else {
            pthread_cond_broadcast(&p->readers);
        }
    } else {  // Читатель разблокирует
        p->r_count--;
        if (p->r_count == 0 && p->w_lock_count > 0) {
            pthread_cond_signal(&p->writers);
        }
    }

    pthread_mutex_unlock(&p->mutex);
}