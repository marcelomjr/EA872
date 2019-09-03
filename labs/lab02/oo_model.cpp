/*
 Aluno: Marcelo Martimiano Junior RA: 156531
*/
#include <vector>

#include "oo_model.hpp"

#include <ncurses.h>
#include <iostream>


Corpo::Corpo(float massa, float velocidade, float posicao, float const_elastica, float const_amortecimento) {
  this->massa = massa;
  this->velocidade = velocidade;
  this->posicao = posicao;
  this->const_elastica = const_elastica;
  this->pos_neutra = 50;
  this->const_amortecimento = const_amortecimento;
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
                          (*corpos)[k]->get_const_elastica(),\
                          (*corpos)[k]->get_const_amortecimento()
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
void Tela::write(int i, int j, char character) {
  if (i < 0 || i > this->maxI) {
    return;
  }
  if (j < 0 || j > this->maxJ) {
    return;
  }
  move(i, j);
  echochar(character);

}
void Tela::update() {
  int j, pos_corpo;

  // Desenha corpos na tela
  std::vector<Corpo *> *corpos = this->lista->get_corpos();


  for (int k=0; k < corpos->size(); k++)
  {

    // Encontra a posição do corpo
    pos_corpo = (int) ((*corpos)[k]->get_posicao()) * \
        (this->maxJ / this->maxY);

    // Desenha a mola até chegar na posição encontrada
    for (j = 0; j < pos_corpo - 1; j++){
      this->write(k, j, '~');
    }
    // Desenha uma "caixa", representando o corpo preso a uma mola
    this->write(k, j++, '|');
    this->write(k, j++, '_');
    this->write(k, j++, '|');

    // Limpa o restante da tela
    for (; j < this->maxJ; j++){
      this->write(k, j, ' ');
    }
  }

  // Desenha uma borda, demarcando os limites da tela
  move(this->maxI, 0);
  for (int j = 0; j <= this->maxJ; j++) {
  echochar('=');
  }

  for (int i = 0; i < this->maxI; i++) {
    move(i, this->maxJ);
    echochar('|');
  }

  // Destaca a posição de equilíbrio das molas
  int pos = (int) ((*corpos)[0]->get_posicao_neutra()) * (this->maxJ / this->maxY);
  move(corpos->size(), pos); echochar('|');

  // Atualiza tela
  refresh();
}

void Tela::stop() {
  endwin();
}

Tela::~Tela() {
  this->stop();;
}
