/*
 Aluno: Marcelo Martimiano Junior RA: 156531
*/

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <unistd.h>

#include "oo_model.hpp"

using namespace std::chrono;
uint64_t get_now_ms() {
  return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

int main ()
{
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

  uint64_t t0;
  uint64_t t1;
  uint64_t deltaT;
  uint64_t T;

  int i = 0;

  T = get_now_ms();
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
    }
    else if (c=='a') {
      f->choque(-1);
    }
    else if (c=='q') {
      break;
    }

    std::this_thread::sleep_for (std::chrono::milliseconds(100));

    i++;
  }
  tela->stop();
  return 0;
}
