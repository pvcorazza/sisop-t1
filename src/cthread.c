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

/* Imprime na tela as filas APTO, EXECUTANDO e BLOQUEADO, e o seu conte�do.
   Entre par�nteses, ap�s o tid de cada thread, a prioridade dela.        */
void printFilas() {

    TCB_t *thread;

    printf("\n\nAPTO ");
    if(FirstFila2(&APTO) == 0) {
        do {
            printf("-> ");
            thread = GetAtIteratorFila2(&APTO);
            //printf("%d (%d) ", thread->tid, thread->prio);    Colocar vis�vel depois, quando implementar a prioridade!
            printf("%d ", thread->tid);
        } while(NextFila2(&APTO) == 0);
    }

    printf("\nEXECUTANDO ");
    if(FirstFila2(&EXECUTANDO) == 0) {
        do {
            printf("-> ");
            thread = GetAtIteratorFila2(&EXECUTANDO);
            //printf("%d (%d) ", thread->tid, thread->prio);    Colocar vis�vel depois, quando implementar a prioridade!
            printf("%d ", thread->tid);
        } while(NextFila2(&EXECUTANDO) == 0);
    }

    printf("\nBLOQUEADO ");
    if(FirstFila2(&BLOQUEADO) == 0) {
        do {
            printf("-> ");
            thread = GetAtIteratorFila2(&BLOQUEADO);
            //printf("%d (%d) ", thread->tid, thread->prio);    Colocar vis�vel depois, quando implementar a prioridade!
            printf("%d ", thread->tid);
        } while(NextFila2(&BLOQUEADO) == 0);
    }

    printf("\n\n");
}

/* Dado a fila do semaforo, imprime na tela */
void printSemaforo(csem_t *sem) {

    TCB_t *thread;

    printf("\nFILA DO SEMAFORO (%d = COUNT) ", sem->count);
    if(FirstFila2(sem->fila) == 0) {
        do {
            printf("-> ");
            thread = GetAtIteratorFila2(sem->fila);
            printf("%d ", thread->tid);
        } while(NextFila2(sem->fila) == 0);
    }

    printf("\n\n");
}

/* mode � qual o elemento que se deseja comparar. Se mode == 0, se compara o par�metro passado com o tid da thread. Se mode != 0 compara com awaited_tid.
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

/* Fun��o que coloca novamente uma thread que estava em BLOQUEADO para APTO
   Se mode == 0,  uma thread terminou e precisa testar se existe uma thread que esperava em bloqueado pelo termino dessa thread (awaited_tid = tid) cjoin
   Se mode != 0,  um recurso foi liberado por csignal e precisamos encontrar em BLOQUEADO a primeira thread da fila do sem�foro e coloc�-lo em APTO   */
void checkBLOQUEADO(int tid, int mode) {

    TCB_t *thread;

    if(FirstFila2(&BLOQUEADO) == 0) {      // se fila bloqueado n�o est� vazia
        do {

            if(mode == 0) {
                thread = GetAtIteratorFila2(&BLOQUEADO);
                if(thread->awaited_tid == tid) {          // se h� uma thread que esperava pelo termino dessa thread que terminou
                    if(AppendFila2(&APTO, (void *) thread) == 0) {
                        thread->state = PROCST_APTO;
                        thread->awaited_tid = -1;
                        DeleteAtIteratorFila2(&BLOQUEADO);
                        break;
                    }
                }
            }

            else {
                thread = GetAtIteratorFila2(&BLOQUEADO);
                if(thread->tid == tid) {                               // se achou, em bloqueado, a primeira thread da fila do sem�foro com tid passado como par�metro
                    if(AppendFila2(&APTO, (void *) thread) == 0) {     // se conseguiu inserir nova thread na fila de aptos
                        thread->state = PROCST_APTO;
                        DeleteAtIteratorFila2(&BLOQUEADO);
                        break;
                    }
                }
            }

        } while(NextFila2(&BLOQUEADO) == 0);
    }
}

void dispatcher() {

    TCB_t *thread;

    if(!cyield_cjoin_cwait_ToDispatcher) {  // se veio para o dispatcher porque terminou sua execu��o (uc_link = &escalonador)
        if(FirstFila2(&EXECUTANDO) == 0) {        // se h� alguma thread executando para colocar em apto
            thread = GetAtIteratorFila2(&EXECUTANDO);
            thread->state = PROCST_TERMINO;
            DeleteAtIteratorFila2(&EXECUTANDO);
            checkBLOQUEADO(thread->tid, 0);   // como a thread terminou, verificar em BLOQUEADO se alguma thread esperava por ela (cjoin!)
            free(thread);
        }
    }

    cyield_cjoin_cwait_ToDispatcher = 0;

    if(FirstFila2(&APTO) == 0) {      // se fila de aptos n�o est� vazia
        thread = GetAtIteratorFila2(&APTO);
        thread->state = PROCST_EXEC;
        AppendFila2(&EXECUTANDO, (void *) thread);
        DeleteAtIteratorFila2(&APTO);
        printf("\ndispatcher:");
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
            printf("\nERRO: Falha ao criar a thread da main.\n");

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
        printf("\nERRO: N�o foi possivel criar contexto para a thread %d.\n", new_thread->tid);
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
        printf("\nERRO: N�o foi possivel inserir a nova thread %d na fila de aptos.\n", new_thread->tid);
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
                        thread->awaited_tid = tid;
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

    printf("\nERRO: N�o foi possivel bloquear a thread %d.\n", thread->tid);
    return -1;
}

int csem_init(csem_t *sem, int count) {

    if(first_time) {
        init();
        first_time = 0;
    }

    sem->count = count;
    sem->fila = malloc(sizeof(FILA2));

    if(CreateFila2(sem->fila) == 0) {       // se conseguiu criar a fila do sem�foro
        //sem->fila = NULL;                 // coloca em NULL pra poder testar se foi inicializado antes, em cwait e csignal
        return 0;
    }

    printf("\nERRO: N�o foi possivel inicializar o semaforo.\n");
    return -1;
}

int cwait(csem_t *sem) {

    //if(sem->fila == NULL) {      // se sem�foro n�o criado

        if(sem->count > 0) {       // se recurso est� livre
            sem->count--;
            printf("\ncwait da thread executando (recurso estava disponivel).\n");
            return 0;
        }

        else {                    // se recurso est� ocupado, vai para bloqueado
            if(FirstFila2(&EXECUTANDO) == 0) {        // se h� alguma thread executando para colocar em apto
                TCB_t *thread = GetAtIteratorFila2(&EXECUTANDO);
                if(AppendFila2(&BLOQUEADO, (void *) thread) == 0) {     // se conseguiu inserir nova thread na fila de bloqueado
                    thread->state = PROCST_BLOQ;
                    DeleteAtIteratorFila2(&EXECUTANDO);
                    AppendFila2(sem->fila, (void *) thread);
                    sem->count--;
                    cyield_cjoin_cwait_ToDispatcher = 1;
                    printf("\ncwait de %d.", thread->tid);
                    printSemaforo(sem);
                    swapcontext(&thread->context, &escalonador);
                    return 0;
                }
            }
        }
   //}

    printf("ERRO: Semaforo precisa ser inicializado.\n");
    return -1;
}

int csignal(csem_t *sem) {

    //if(sem->fila == NULL) {      // se sem�foro n�o criado

        sem->count++;

        if(FirstFila2(sem->fila) == 0) {                       // se h� algu�m na fila do sem�foro
            TCB_t *thread = GetAtIteratorFila2(sem->fila);     // pega o primeiro da fila do sem�foro
            checkBLOQUEADO(thread->tid, 1);                    // coloca a thread de BLOQUEADO para APTO
            DeleteAtIteratorFila2(sem->fila);                  // deleta a thread da fila do sem�foro
        }

        printf("\ncsignal da thread executando.");
        printSemaforo(sem);
        return 0;

    //}

    printf("ERRO: Semaforo precisa ser inicializado.\n");
    return -1;
}

int cidentify (char *name, int size);
