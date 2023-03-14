#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <signal.h>
#include <unistd.h>



// Define the function to be called when ctrl-c (SIGINT) is sent to process
void signal_callback_handler(int signum) {
   std::cout << "Caught signal " << signum << "\n";
   // Terminate program
   exit(signum);
}

int main()
{
     // Register signal and signal handler
   signal(SIGINT, signal_callback_handler);

    std::cout << "starting timer\n";
    sleep(10);

   



}