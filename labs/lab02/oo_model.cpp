#include <vector>

#include "oo_model.hpp"

#include <ncurses.h>
#include <iostream>


Corpo::Corpo(float massa, float velocidade, float posicao, float const_elastica) {
  this->massa = massa;
  this->velocidade = velocidade;
  this->posicao = posicao;
  this->const_elastica = const_elastica;
  this->pos_neutra = 50;
  this->const_amortecimento = 1;
}

void Corpo::update(float nova_velocidade, float nova_posicao) {
  this->velocidade = nova_velocidade;
  this->posicao = nova_posicao;
}

float Corpo::get_massa() {
  return this->massa;
}

float Corpo::get_velocidade() {
  return this->velocidade;
}

float Corpo::get_posicao() {
  return this->posicao;
}

float Corpo::get_posicao_neutra() {
  return this->pos_neutra;
}

float Corpo::get_const_elastica() {
  return this->const_elastica;
}

float Corpo::get_const_amortecimento() {
  return this->const_amortecimento;
}

ListaDeCorpos::ListaDeCorpos() {
  this->corpos = new std::vector<Corpo *>(0);
}

void ListaDeCorpos::hard_copy(ListaDeCorpos *ldc) {
  std::vector<Corpo *> *corpos = ldc->get_corpos();

  for (int k=0; k<corpos->size(); k++) {
    Corpo *c = new Corpo( (*corpos)[k]->get_massa(),\
                          (*corpos)[k]->get_posicao(),\
                          (*corpos)[k]->get_velocidade(),\
                          (*corpos)[k]->get_const_elastica()
                        );
    this->add_corpo(c);
  }

}


void ListaDeCorpos::add_corpo(Corpo *c) {
  (this->corpos)->push_back(c);
}

std::vector<Corpo*> *ListaDeCorpos::get_corpos() {
  return (this->corpos);
}

Fisica::Fisica(ListaDeCorpos *ldc) {
  this->lista = ldc;
}

void Fisica::update(float deltaT) {

  // converte deltaT para segundos
  deltaT /= 1000;
  // Atualiza parametros dos corpos!
  std::vector<Corpo *> *c = this->lista->get_corpos();
  for (int i = 0; i < (*c).size(); i++) {

    float pos_anterior = (*c)[i]->get_posicao();
    float vel_anterior = (*c)[i]->get_velocidade();
    float pos_neutra   = (*c)[i]->get_posicao_neutra();

    float alfa = (-(*c)[i]->get_const_elastica() / (*c)[i]->get_massa());
    float beta = (-(*c)[i]->get_const_amortecimento() / (*c)[i]->get_massa());

    float delta_vel = deltaT * (alfa * (pos_anterior - pos_neutra) + beta * vel_anterior);
    float new_vel   = vel_anterior + delta_vel;
    // A nova posição é aproximada pela velocidade média no intervalo de tempo deltaT
    float new_pos   = pos_anterior + 0.5 * (vel_anterior + new_vel) * deltaT;

    //float new_vel = (*c)[i]->get_velocidade() + (float)deltaT * (-10.0)/1000;
    //float new_pos = (*c)[i]->get_posicao() + (float)deltaT * new_vel/1000;
    // Limite da mola
    if (new_pos > 20) {
      //new_pos = 20;
      //new_vel *= -1;
    }
    //std::cout << new_pos << " vel: " << new_vel << "   delta_vel:" << delta_vel <<"   Fel:" << alfa * (pos_anterior - pos_neutra) << "\n";

    (*c)[i]->update(new_vel, new_pos);
  }
}

Tela::Tela(ListaDeCorpos *ldc, int maxI, int maxJ, float maxX, float maxY) {
  this->lista = ldc;
  this->lista_anterior = new ListaDeCorpos();
  this->lista_anterior->hard_copy(this->lista);
  this->maxI = maxI;
  this->maxJ = maxJ;
  this->maxX = maxX;
  this->maxY = maxY;
}

void Tela::init() {
  initscr();			       /* Start curses mode 		*/
	raw();				         /* Line buffering disabled	*/
  curs_set(0);           /* Do not display cursor */
}

void Tela::update() {
  int j;

  std::vector<Corpo *> *corpos_old = this->lista_anterior->get_corpos();

  // Apaga corpos na tela
  for (int k=0; k<corpos_old->size(); k++)
  {
    j = (int) ((*corpos_old)[k]->get_posicao()) * \
        (this->maxJ / this->maxY);
    move(k, j);   /* Move cursor to position */
    echochar(' ');  /* Prints character, advances a position */
  }

  // Desenha corpos na tela
  std::vector<Corpo *> *corpos = this->lista->get_corpos();

  for (int k=0; k<corpos->size(); k++)
  {
    j = (int) ((*corpos)[k]->get_posicao()) * \
        (this->maxJ / this->maxY);
    move(k, j);   /* Move cursor to position */
    echochar('*');  /* Prints character, advances a position */

    // Atualiza corpos antigos
    (*corpos_old)[k]->update(  (*corpos)[k]->get_velocidade(),\
                               (*corpos)[k]->get_posicao());
  }

  /*Todo: apagar
  move(0, this->maxJ - 4);
  echochar('T');
  echochar('E');
  echochar('T');
  echochar('O');


  move(this->maxI - 1, this->maxJ - 4);
  echochar('C');
  echochar('H');
  echochar('A');
  echochar('O');

  move(0, this->maxJ); echochar('Y');
  for (int j = 0; j < this-> maxJ; j++) {
    move(this->maxI, j);
    echochar('=');
  }*/
  int pos = (int) ((*corpos)[0]->get_posicao_neutra()) * (this->maxI / this->maxX);
  move(3, pos); echochar('-');

  // Atualiza tela
  refresh();
}

void Tela::stop() {
  endwin();
}

Tela::~Tela() {
  this->stop();;
}
