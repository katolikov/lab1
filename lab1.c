#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>
#include <wait.h>

sigjmp_buf obl; //область памяти для сохранения состояния процесса 
void fun(){ //обработка прерывания
    printf("\ninterruption \n");
    system("date +%T"); //вывод времени
    siglongjmp(obl,1); //возвращение на sigsetjmp
}

int main(){
    int fd[2]; //дескрипторы межпроцессного канала
    int ret, s;
    char n[100];
    struct sigaction sa_new;

    sa_new.sa_handler = fun; //при поступлении сигнала, указываем функцию его обработки
    sigprocmask(0,0,&sa_new.sa_mask); //блокировка сигналов
    sa_new.sa_flags = 0; //флаги, управляющие доставкой сигнала
    sigaction(SIGINT,&sa_new,0); //метод обработка сигнала
    sigsetjmp(obl,1); //запоминание текущего состояния
    printf("Waiting for interruption\n");

    sleep(5);
    if(pipe(fd)==-1) //создание межпроцессного канала
    {
        printf("Pipe error\n");
        exit(1);
    }
    switch(fork()){ //распараллеливание процесса
        case -1: //ошибка
            printf("Fork error\n");
            exit(1);
        case 0: //процесс-потомок
            close(fd[0]);
            dup2(fd[1],1); //дублирование дескриптора на стандартный вывод
            execl("/usr/bin/users","users",NULL);
            exit(1);
        default:  //процесс-родитель
            wait(&s); //ожидание окончания процесса-потомка
            ret = read(fd[0],n,99);
            n[ret] = '\0';
            printf("\nUsers:");
            printf("%s",n); //вывод строки
    }
    return 0;
}


