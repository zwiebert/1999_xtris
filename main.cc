#include <stdlib.h>
#include "dummies.hh"
#include "playfield.hh"
#include "gui_xt.hh"
#include "stones.hh"
#include <unistd.h>

struct Tetris
{
  Tetris (int *argc_ret, char *argv[], Stone *stones[]):
    field (10, 25),
    view (&field, argc_ret, argv),
    ctrl (view, stones),
    nmb_stones (0)
    {
      while (stones[nmb_stones])
	++nmb_stones;
      stone_set = new Stone *[nmb_stones];
      for (unsigned i=0; i < nmb_stones; ++i)
	stone_set[i] = stones[i];
    }
  play ();
private:
  Tetris_Play_Field field;
  PF_VIEW view;
  PF_CTRL ctrl;
  Stone **stone_set;
  unsigned nmb_stones;
};

int
Tetris::play ()
{
  srandom ((unsigned) time (0));
  unsigned i = random () % nmb_stones;
  Stone &stone = *stone_set[i];
  field.set_current_stone (stone);
  field.notify_views ();
  ctrl.process_events (true);
  return 0; // success
}

static Tetris *tetris;
void delete_tetris () { delete tetris; }

int
main (int ac, char *av[])
{
  Q_Stone qs;
  T_Stone ts;
  I_Stone is;
  L_Stone ls;
  J_Stone js; 
  S_Stone ss; 
  Z_Stone zs; 
  Stone *stone_set[] = {&ls, &js, &qs, &ts, &is, &ss, &zs, 0};
  tetris = new Tetris (&ac, av, stone_set);
  atexit (delete_tetris);
  tetris->play ();
  sleep (1);
}
