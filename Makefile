all: Makefile a.out 

a.out: Makefile  obj/ClientConversation.o obj/main.o obj/NetJobs.o obj/MasterConversation.o obj/Conversation.o 
	g++ -std=c++17  obj/ClientConversation.o obj/main.o obj/NetJobs.o obj/MasterConversation.o obj/Conversation.o -o a.out

obj/ClientConversation.o: src/ClientConversation.cpp \
 include/Conversation.hpp Makefile
	g++ -I . -I ./include -I ./obj -I ./src -Wall -Werror -std=c++17 -c ./src/ClientConversation.cpp -o obj/ClientConversation.o

obj/main.o: src/main.cpp include/NetJobs.hpp include/Conversation.hpp Makefile
	g++ -I . -I ./include -I ./obj -I ./src -Wall -Werror -std=c++17 -c ./src/main.cpp -o obj/main.o

obj/NetJobs.o: src/NetJobs.cpp include/NetJobs.hpp \
 include/Conversation.hpp Makefile
	g++ -I . -I ./include -I ./obj -I ./src -Wall -Werror -std=c++17 -c ./src/NetJobs.cpp -o obj/NetJobs.o

obj/MasterConversation.o: src/MasterConversation.cpp \
 include/Conversation.hpp Makefile
	g++ -I . -I ./include -I ./obj -I ./src -Wall -Werror -std=c++17 -c ./src/MasterConversation.cpp -o obj/MasterConversation.o

obj/Conversation.o: src/Conversation.cpp include/Conversation.hpp Makefile
	g++ -I . -I ./include -I ./obj -I ./src -Wall -Werror -std=c++17 -c ./src/Conversation.cpp -o obj/Conversation.o

