/*
 Aluno: Marcelo Martimiano Junior RA: 156531
*/

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <unistd.h>

#include "oo_model.hpp"
#include "01-playback.hpp"

using namespace std::chrono;

volatile int audio_thread_running = 0;
Audio::Sample *asample;
Audio::Player *player;


uint64_t get_now_ms() {
  return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

void* audio_thread_handler(void* params) {
  uint64_t t0;
  uint64_t t1;
  if (audio_thread_running == 1) {
  	return NULL;
  }
  
  player->play(asample);
  
  while (1) {
   std::this_thread::sleep_for (std::chrono::milliseconds(1));
   t1 = get_now_ms();
   if (t1-t0 > 500) break;
  }
  asample->set_position(0);
  
  audio_thread_running = 0;
  
  return NULL;
}

int main ()
{
  uint64_t t2, t3;
  uint64_t t0;
  uint64_t t1;
  uint64_t deltaT;
  uint64_t T = get_now_ms();
  pthread_t audio_thread;


  asample = new Audio::Sample();
  asample->load("assets/blip.dat");

  player = new Audio::Player();
  player->init();	
  
  while (1) {
    std::this_thread::sleep_for (std::chrono::milliseconds(1));
    t1 = get_now_ms();
    if (t1-t0 > 500) break;
  }
  pthread_create(&audio_thread, NULL, audio_thread_handler, NULL); 
  std::cout << "Audio initialized!\n";
  	
  // massa, velocidade, posicao, const_elastica, const_amortecimento
  Corpo *c1 = new Corpo(1, 0, 180, 4, 0.8);
  Corpo *c2 = new Corpo(1, 0, 180, 4, 1.0);
  Corpo *c3 = new Corpo(1, 0, 180, 4, 1.5);
  Corpo *c4 = new Corpo(1, 0, 180, 4, 2);

  ListaDeCorpos *l = new ListaDeCorpos();
  l->add_corpo(c1);
  l->add_corpo(c2);
  l->add_corpo(c3);
  l->add_corpo(c4);

  Fisica *f = new Fisica(l);

  Tela *tela = new Tela(l, 20, 100, 200, 200);
  tela->init();
  
  Teclado *teclado = new Teclado();
  teclado->init();

  t1 = T;
  while (1) {
    // Atualiza timers
    t0 = t1;
    t1 = get_now_ms();
    deltaT = t1-t0;

    // Atualiza modelo
    f->update(deltaT);

    // Atualiza tela
    tela->update();

    // Lê o teclado
    char c = teclado->getchar();
    if (c=='d') {
      f->choque(1);
      
      if (audio_thread_running != 1) {
      	// Toca som a teclar "d"
      	pthread_create(&audio_thread, NULL, audio_thread_handler, NULL); 
      }
    }
    else if (c=='a') {
	   if (audio_thread_running != 1) {
         // Toca som a teclar "a"
         pthread_create(&audio_thread, NULL, audio_thread_handler, NULL);
      }
      f->choque(-1);
    }
    else if (c=='q') {
      break;
    }

    std::this_thread::sleep_for (std::chrono::milliseconds(100));

  }
  tela->stop();

  pthread_join(audio_thread, NULL);
  
  player->stop();

  
  return 0;
}
