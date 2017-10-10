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

int thread_id = 1;   // vari�vel que fornece o id das threads, incrementando a cada chamada de "ccreate" caso consiga criar
int first_time = 1;  // vari�vel que indica ser a primeira vez que uma thread � criada. Se sim, em "ccreate", precisa inicializar tudo
int cyield_cjoin_cwait_ToDispatcher = 0;  // flag pra verificar se foi para o dispatcher por causa de um cyield, cjoin ou cwait. 0 se n�o foi, portanto foi pro dispatcher porque terminou sua execu��o

void printFilas() {

    TCB_t *thread;

    printf("\n\nAPTO ");
    if(FirstFila2(&APTO) == 0) {
        do {
            printf("-> ");
            thread = GetAtIteratorFila2(&APTO);
            printf("%d ", thread->tid);
        } while(NextFila2(&APTO) == 0);
    }

    printf("\nEXECUTANDO ");
    if(FirstFila2(&EXECUTANDO) == 0) {
        do {
            printf("-> ");
            thread = GetAtIteratorFila2(&EXECUTANDO);
            printf("%d ", thread->tid);
        } while(NextFila2(&EXECUTANDO) == 0);
    }

    printf("\nBLOQUEADO ");
    if(FirstFila2(&BLOQUEADO) == 0) {
        do {
            printf("-> ");
            thread = GetAtIteratorFila2(&BLOQUEADO);
            printf("%d ", thread->tid);
        } while(NextFila2(&BLOQUEADO) == 0);
    }

    printf("\n\n");
}

/* mode � qual o elemento que se deseja comparar. Se 0, se compara o par�metro passado com o tid da thread. Sen�o, compara-o com awaited_tid.
   Retorna 0 se n�o achou. Caso contr�rio, retorna 1.*/
int tidIsIn(int tid, PFILA2 fila, int mode) {

    if(FirstFila2(fila) == 0) {    // se fila n�o est� vazia
        TCB_t *thread;
        do {

            thread = GetAtIteratorFila2(fila);

            if(mode == 0) {
                if(thread->tid == tid)          // a thread existe
                    return 1;
            }

            else {
                if(thread->awaited_tid == tid)
                    return 1;                    // j� existe uma thread esperando pelo tid passado
            }

        } while(NextFila2(fila) == 0);
    }

    return 0;
}

void dispatcher() {

    TCB_t *thread;

    if(!cyield_cjoin_cwait_ToDispatcher) {  // se veio para o dispatcher porque terminou sua execu��o (uc_link = &escalonador)
        if(FirstFila2(&EXECUTANDO) == 0) {        // se h� alguma thread executando para colocar em apto
            thread = GetAtIteratorFila2(&EXECUTANDO);
            thread->state = PROCST_TERMINO;
            DeleteAtIteratorFila2(&EXECUTANDO);
            free(thread);
            // como thread terminou, ver aqui se tinha algu�m em bloqueado esperando por ela, e caso sim, botar pra apto novamente! (cjoin)
        }
    }

    cyield_cjoin_cwait_ToDispatcher = 0;

    if(FirstFila2(&APTO) == 0) {      // se fila de aptos n�o est� vazia
        thread = GetAtIteratorFila2(&APTO);
        thread->state = PROCST_EXEC;
        AppendFila2(&EXECUTANDO, (void *) thread);
        DeleteAtIteratorFila2(&APTO);
        printf("\ndispatcher");
        printFilas();
        setcontext(&thread->context);
    }
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
            printf("ERRO: Falha ao criar a thread da main.\n");

    }
}

int ccreate (void* (*start)(void*), void *arg, int prio) {

    if(first_time) {
        init();
        first_time = 0;
    }

    TCB_t * new_thread = malloc(sizeof(TCB_t));

    new_thread->tid = thread_id;
    new_thread->state = PROCST_CRIACAO;
    new_thread->prio = prio;
    new_thread->awaited_tid = -1;       // como n�o fez cjoin, n�o espera por nenhuma thread

    if(getcontext(&new_thread->context) == 0) {         // se conseguiu inicializar o contexto da nova thread
        new_thread->context.uc_stack.ss_size = SIGSTKSZ;
        new_thread->context.uc_stack.ss_sp = malloc(SIGSTKSZ);
        new_thread->context.uc_link = &escalonador;     // contexto que ser� executado depois que terminar execu��o
        makecontext(&new_thread->context, (void (*)(void)) start, 1, arg);
    }

    else {       // n�o foi conseguiu inicializar o contexto da nova thread
        printf("ERRO: N�o foi poss�vel criar contexto para a thread %d.\n", new_thread->tid);
        free(new_thread);
        return -1;
    }

    if(AppendFila2(&APTO, (void *) new_thread) == 0) {     // se conseguiu inserir nova thread na fila de aptos
        new_thread->state = PROCST_APTO;
        thread_id ++;
        printf("\nccreate de %d.", new_thread->tid);
        printFilas();
        return new_thread->tid;
    }

    else {       // se n�o foi poss�vel inserir a thread na fila de aptos
        printf("ERRO: N�o foi poss�vel inserir a nova thread %d na fila de aptos.\n", new_thread->tid);
        free(new_thread);
        return -1;
    }
}

int cyield(void) {

    if(FirstFila2(&EXECUTANDO) == 0) {        // se h� alguma thread executando para colocar em apto
        TCB_t *thread = GetAtIteratorFila2(&EXECUTANDO);
        if(AppendFila2(&APTO, (void *) thread) == 0) {     // se conseguiu inserir nova thread na fila de aptos
            thread->state = PROCST_APTO;
            DeleteAtIteratorFila2(&EXECUTANDO);
            cyield_cjoin_cwait_ToDispatcher = 1;
            printf("\ncyield de %d.", thread->tid);
            printFilas();
            swapcontext(&thread->context, &escalonador);
            return 0;
        }
    }

    return -1;
}

int cjoin(int tid) {

    if(first_time) {
        init();
        first_time = 0;
    }

    TCB_t *thread;

    if(FirstFila2(&EXECUTANDO) == 0) {
        thread = GetAtIteratorFila2(&EXECUTANDO);
        if(thread->tid != tid) {                                    // se a thread n�o est� tentando fazer cjoin dela mesma
            if(!tidIsIn(tid, &BLOQUEADO, 1)) {                         // se n�o existe uma thread bloqueada esperando por uma outra thread com esse tid
                if(tidIsIn(tid, &APTO, 0) || tidIsIn(tid, &BLOQUEADO, 0)) {  // se a thread que ela vai esperar existe, estando ou em APTO ou BLOQUEADO
                    if(AppendFila2(&BLOQUEADO, (void *) thread) == 0) {        // se conseguiu inserir nova thread na fila de bloqueados
                        DeleteAtIteratorFila2(&EXECUTANDO);
                        thread->state = PROCST_BLOQ;
                        cyield_cjoin_cwait_ToDispatcher = 1;
                        printf("\ncjoin de %d.", thread->tid);
                        printFilas();
                        swapcontext(&thread->context, &escalonador);
                        return 0;
                    }
                }
            }
        }
    }

    printf("ERRO: N�o foi poss�vel bloquear a thread %d.\n", thread->tid);
    return -1;
}

int csem_init(csem_t *sem, int count);

int cwait(csem_t *sem);

int csignal(csem_t *sem);

int cidentify (char *name, int size);
