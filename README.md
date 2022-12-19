# TCP_Congestion_Control 
The project contains a sender and a receiver. The sender sends a 1MB TXT file. Splits the file into two parts. 50% of the total size. Send the first part using CUBIC algorithms according to the general definition for SOCKET and the second part is sent according to RENO algorithms. With an average and possible inside a ZIP file there are tracking files for countries with different percentages of loss.

## How to run 
- clone this project in vsc in linux OS
- cd src
- gcc receiver.c -o receiver
- ./receiver
- gcc sender.c -o sender
- ./sender
