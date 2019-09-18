FLAGS=-lncurses -std=c++11 -lpthread -lportaudio

all: model

model: model_mainloop.cpp oo_model.cpp oo_model.hpp 01-playback.cpp
	g++ -omodel model_mainloop.cpp oo_model.cpp 01-playback.cpp $(FLAGS)

clean:
	rm model
