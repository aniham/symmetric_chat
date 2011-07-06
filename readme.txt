This lightweight program serves as a chat client and chat server simultaneously.

NOTE: Linux/Mac OS only, local network only

1. To talk from two terminals on the same system save chat_1.c, chat_2.c, and shared.h  and the same directory; for different systems, each will need shared.h and either chat_1.c or chat_2.c
2. Open chat_1.c and chat_2.c and change the send/recv IP address and port number.  The defaults are localhost and 5988/5989.  Also change user to whatever name you want you users to be called.
3. You should be able to build by typing make chat_1.  If for some reason this doesnt work, use gcc chat1.c -o chat_1 -lpthread
4. Once you've built chat_1 and chat_2, run them in separate terminals.  You'll be able to type messages on one end and receive them on the other. NOTE: in order for the users to connect properly, the two sessions should be run within 10 seconds of each other.