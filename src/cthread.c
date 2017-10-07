/*
 * Universidade Federal do Rio Grande do Sul (UFRGS)
 * Sistemas Operacionais I - Trabalho 1
 *
 * Giovani Tirello, Marcelo Wille, Paulo Victor Corazza
 */

#include "../include/support.h"
#include "../include/cdata.h"
#include "../include/cthread.h"

#include <stdlib.h>
#include <stdio.h>

ucontext_t escalonador;

FILA2 APTO;
FILA2 EXECUTANDO;
FILA2 BLOQUEADO;

int thread_id = 1;   // variável que fornece o id das threads, incrementando a cada chamada de "ccreate" caso consiga criar
int first_time = 1;  // variável que indica ser a primeira vez que uma thread é criada. Se sim, em "ccreate", precisa inicializar tudo

void dispatcher() {

    if(FirstFila2(&APTO) == 0) {   // se fila de aptos não está vazia
        TCB_t *thread = GetAtIteratorFila2(&APTO);
        thread->state = PROCST_EXEC;
        AppendFila2(&EXECUTANDO, (void *) thread);
        DeleteAtIteratorFila2(&APTO);
        setcontext(&thread->context);
    }

    printf("Escalonou!\n");
}

void init() {

    if(CreateFila2(&APTO) == 0 && CreateFila2(&EXECUTANDO) == 0 && CreateFila2(&BLOQUEADO) == 0) {  // se criou as filas corretamente;

        //inicializa escalonador
        getcontext(&escalonador);
        escalonador.uc_link = 0;   // se o escalonador executar "dispatcher" sem mudar contexto, vai voltar pra main
        escalonador.uc_stack.ss_size = SIGSTKSZ;
        escalonador.uc_stack.ss_sp = malloc(SIGSTKSZ);
        makecontext(&escalonador, (void(*)(void)) dispatcher, 0);

        //inicializa main como thread de tid=0
        TCB_t * main_thread = malloc(sizeof(TCB_t));
        main_thread->tid = 0;
        main_thread->state = PROCST_EXEC;
        main_thread->prio = 0;
        getcontext(&main_thread->context);
        if(AppendFila2(&EXECUTANDO, (void *) main_thread) != 0)
            printf("ERRO: Falha ao criar a thread da main!\n");

    }
}

int ccreate (void* (*start)(void*), void *arg, int prio) {

    if(first_time) {
        init();
        printf("Entrou em init();\n");
        first_time = 0;
    }

    TCB_t * new_thread = malloc(sizeof(TCB_t));

    new_thread->tid = thread_id;
    new_thread->state = PROCST_CRIACAO;
    new_thread->prio = prio;

    if(getcontext(&new_thread->context) == 0) {         // se conseguiu inicializar o contexto da nova thread
        printf("\nCriou o contexto!\n");
        new_thread->context.uc_stack.ss_size = SIGSTKSZ;
        new_thread->context.uc_stack.ss_sp = malloc(SIGSTKSZ);
        new_thread->context.uc_link = &escalonador;     // contexto que será executado depois que terminar execução
        makecontext(&new_thread->context, (void (*)(void)) start, 1, arg);
    }

    else {       // não foi conseguiu inicializar o contexto da nova thread
        printf("ERRO: Não foi possível criar contexto para a thread %d.\n", new_thread->tid);
        free(new_thread);
        return -1;
    }

    if(AppendFila2(&APTO, (void *) new_thread) == 0) {     // se conseguiu inserir nova thread na fila de aptos
        new_thread->state = PROCST_APTO;
        thread_id ++;
        printf("Criou thread %d com sucesso.\n", new_thread->tid);
        return new_thread->tid;
    }

    else {       // se não foi possível inserir a thread na fila de aptos
        printf("ERRO: Não foi possível inserir a nova thread %d na fila de aptos.\n", new_thread->tid);
        free(new_thread);
        return -1;
    }
}

int cyield(void) {

    // precisa considerar também a inicialização, assim como em "ccreate"? Digamos que um usuário faça cyield() antes.

    if(FirstFila2(&EXECUTANDO) == 0) {        // se há alguma thread executando para colocar em apto
        TCB_t *thread = GetAtIteratorFila2(&EXECUTANDO);
        thread->state = PROCST_APTO;
        DeleteAtIteratorFila2(&EXECUTANDO);
        if(AppendFila2(&APTO, (void *) thread) == 0) {     // se conseguiu inserir nova thread na fila de aptos
            swapcontext(&thread->context, &escalonador);
            return 0;
        }
    }
    return -1;
}

int cjoin(int tid);

int csem_init(csem_t *sem, int count);

int cwait(csem_t *sem);

int csignal(csem_t *sem);

int cidentify (char *name, int size);
