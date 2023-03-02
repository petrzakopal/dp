// @file thread.cpp
// compilation command:  gcc -std=c++14 thread.cpp -o thread  -lstdc++ -lpthread

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <future>
#include <chrono>
#include <thread>
#include <mutex>

int threadLoopOutput = 0;
bool isDataFromBackgroundThreadReady = false; // zde je použito proto aby se zajistilo, že nejsou použity v main while smyšce programu na pozadí dvě stejné hodnoty - když nedošlo vlastně ještě k zápisu, jinak kdyby se mohli použít, tak se nebude vůbec tato proměnná v if statements používat
std::mutex gLock;


// background thread which works as a timer
void threadLoop()
{   int i = 0; // timer counter
    while(true) // free running timer
    {
        if(i == 500) // timer overflow value
        {   
            // if timer has finished (interrupt has risen)
            // copy data / insert data to shared variable
            if(isDataFromBackgroundThreadReady == false)
            {
                
                i = 0;
                gLock.lock(); // mutex locking - any other thread can't access this variable (think it cannot write or read)
                threadLoopOutput = threadLoopOutput + 1; // edit the shared variable
                gLock.unlock(); // mutex unlock
                isDataFromBackgroundThreadReady = true; // flag to main while loop that new data is present

            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));   // just emulate some time delay
            
            
        }

        i++;

    }
}


// main function
int main()
{
    
    std::thread backgroundThread(&threadLoop); // initiate a new background Thread
   
    while(true) // main while loop as in low lvl embedded proc
    {
        std::cout << "Main thread is running!\n"; // just something to see
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // simulating some tasks in the background

        // there is new data present from background Thread
        if(isDataFromBackgroundThreadReady)
        {
            
            // lock the shared resource so any other thread cannot acces it - does not have to be used there, because the isDataFromBackgroundThreadReady prevents thread from entering data inserting
            gLock.lock();
            std::cout << "Background thread output is: " << threadLoopOutput << "\n";
            gLock.unlock();
            isDataFromBackgroundThreadReady = false;

        }


    }
    backgroundThread.join();

    
    return(0);
}