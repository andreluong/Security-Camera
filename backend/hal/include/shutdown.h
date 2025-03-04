// HAL component to shut down threads 
// Decide whether to use pthreads ot std::thread (subject to change)
#include <thread>
#include "pthread.h"

void Shutdown_waitForShutdown(pthread_t thread) {
    pthread_join(thread, NULL);
}

void Shutdown_waitForShutdown(std::thread thread) {
    thread.join();
}

