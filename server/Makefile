 #enums/GameState/GameStateUtils.cpp Buffer/Buffer.cpp Buffer/ReadBuffer.cpp Buffer/WriteBuffer.cpp Handler/Handler.cpp Packet/Packet.cpp Client/Client.cpp Game/Team.cpp Game/Game.cpp Server/Server.cpp main.cpp


TARGET ?= server_geoguessr



SRCS := ./enums/GameState/GameStateUtils.cpp ./Buffer/Buffer.cpp ./Buffer/ReadBuffer.cpp ./Buffer/WriteBuffer.cpp ./Handler/Handler.cpp ./Packet/Packet.cpp ./Client/Client.cpp ./Game/Team.cpp ./Game/Game.cpp ./Server/Server.cpp ./main.cpp
OBJS := $(addsuffix .o,$(basename $(SRCS)))
CFLAGS  = -g -Wall -pthread -std=c++17
CC = g++

$(TARGET): 
	$(CC) $(SRCS) -o $@

