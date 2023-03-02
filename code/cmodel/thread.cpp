#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <future>
#include <chrono>
#include <thread>

int threadLoopOutput = 0;
bool isDataFromBackgroundThreadReady = false;


void threadLoop()
{   int i = 0;
    while(i<1000)
    {
        if(i == 500)
        {   
            isDataFromBackgroundThreadReady = true;
            i = 0;
            threadLoopOutput = threadLoopOutput + 1;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            
        }

        i++;

    }
}

int main()
{
    
    std::thread backgroundThread(&threadLoop);
   
    while(true)
    {
        std::cout << "Main thread is running!\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        if(isDataFromBackgroundThreadReady)
        {
            isDataFromBackgroundThreadReady = false;
            std::cout << "Background thread output is: " << threadLoopOutput << "\n";
        }
        


    }
    backgroundThread.join();
    return(0);
}