#ifndef GGGGT_NATIVE_SEM_WINDOWS_H
#define GGGGT_NATIVE_SEM_WINDOWS_H 1

#include <windows.h>

#include <sys/types.h>

typedef HANDLE ggt_native_sem_t;

#define ggt_native_sem_init(sem, ct) (*(sem)=CreateSemaphore(NULL,(ct),INT_MAX,NULL))
#define ggt_native_sem_destroy(sem) (CloseHandle(*(sem)))
#define ggt_native_sem_post(sem) (ReleaseSemaphore(*(sem),1,NULL))
#define ggt_native_sem_wait(sem) (WaitForSingleObject(*(sem),INFINITE))
#define ggt_native_sem_trywait(sem) ((WaitForSingleObject(*(sem),0)==WAIT_FAILED)?-1:0)

#endif
