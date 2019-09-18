/*
 Aluno: Marcelo Martimiano Junior RA: 156531
*/
#include <vector>

#include "oo_model.hpp"
#include <thread>
#include <ncurses.h>
#include <iostream>


Corpo::Corpo(float massa, float velocidade, float posicao, float const_elastica, float const_amortecimento) {
  this->massa = massa;
  this->velocidade = velocidade;
  this->posicao = posicao;
  this->const_elastica = const_elastica;
  this->pos_neutra = 100;
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

    float new_vel  = vel_anterior + delta_vel;

    float new_pos   = pos_anterior + new_vel * deltaT;

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

void Fisica::choque(int direcao) {
  // Atualiza parametros dos corpos!
  std::vector<Corpo *> *c = this->lista->get_corpos();
  for (int i = 0; i < (*c).size(); i++) {
    float new_vel = (*c)[i]->get_velocidade() + direcao * 20;
    float new_pos = (*c)[i]->get_posicao();
    (*c)[i]->update(new_vel, new_pos);
  }
}

/*
class Teclado {
  private:
    char ultima_captura;
    int rodando;

  public:
    Teclado();
    ~Teclado();
    void stop();
    void init();
    char getchar();
}

*/

void threadfun (char *keybuffer, int *control)
{
  char c;
  while ((*control) == 1) {
    c = getch();
    if (c!=ERR) (*keybuffer) = c;
    else (*keybuffer) = 0;
    std::this_thread::sleep_for (std::chrono::milliseconds(10));
  }
  return;
}

Teclado::Teclado() {
}

Teclado::~Teclado() {
}

void Teclado::init() {
  // Inicializa ncurses
  raw();				         /* Line buffering disabled	*/
	keypad(stdscr, TRUE);	 /* We get F1, F2 etc..		*/
	noecho();			         /* Don't echo() while we do getch */
  curs_set(0);           /* Do not display cursor */

  this->rodando = 1;
  std::thread newthread(threadfun, &(this->ultima_captura), &(this->rodando));
  (this->kb_thread).swap(newthread);
}

void Teclado::stop() {
  this->rodando = 0;
  (this->kb_thread).join();
}

char Teclado::getchar() {
  char c = this->ultima_captura;
  this->ultima_captura = 0;
  return c;
}
