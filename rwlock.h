#ifndef _RWLOCK_H_
#define _RWLOCK_H_

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


void rwlock_init(my_rwlock_t* p);

void rwlock_destroy(my_rwlock_t* p);

void rdlock(my_rwlock_t* p);

void wrlock(my_rwlock_t* p);

void unlock(my_rwlock_t* p);

#endif