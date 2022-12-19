# TCP_Congestion_Control_Algo
The project contains a sender and a receiver. The sender sends a 1MB TXT file(you can send every file you want ). Splits the file into two parts. 50% of the total size. Send the first part using CUBIC algorithms according to the general definition for SOCKET and the second part is sent according to RENO algorithms. With an average and possible inside a ZIP file there are tracking files for countries with different percentages of loss.


## How to Install and Run the Project
- clone this project in vsc in linux OS
- cd src
- gcc receiver.c -o receiver
- ./receiver
- gcc sender.c -o sender
- ./sender
