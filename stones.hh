#ifndef H_TETRIS_STONES_HH_
#define H_TETRIS_STONES_HH_

#include "framework.hh"

const unsigned N_STONES = 8;

class T_Stone : public Stone
{
public:
  T_Stone (): Stone (SA_3)
    {
      bool tmp[4][N_ROWS][N_COLS] = {
	{{1, 1, 1, 0},
	 {0, 1, 0, 0},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}},
	{{0, 1, 0, 0},
	 {1, 1, 0, 0},
	 {0, 1, 0, 0},
	 {0, 0, 0, 0}},
	{{0, 1, 0, 0},
	 {1, 1, 1, 0},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}},
	{{1, 0, 0, 0},
	 {1, 1, 0, 0},
	 {1, 0, 0, 0},
	 {0, 0, 0, 0}}};
      memcpy (itsBitMap, tmp, sizeof (itsBitMap)); //      itsBitMap = tmp;
    }
};

class Q_Stone : public Stone
{
public:
  Q_Stone (): Stone (SA_4)
    {
      bool tmp[4][N_ROWS][N_COLS] = {
	{{1, 1, 0, 0},
	 {1, 1, 0, 0},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}},
	{{1, 1, 0, 0},
	 {1, 1, 0, 0},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}},
	{{1, 1, 0, 0},
	 {1, 1, 0, 0},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}},
	{{1, 1, 0, 0},
	 {1, 1, 0, 0},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}}};
      memcpy (itsBitMap, tmp, sizeof (itsBitMap));
    }
};

class I_Stone : public Stone
{
public:
  I_Stone (): Stone (SA_5)
    {
      bool tmp[4][N_ROWS][N_COLS] = {
	{{1, 1, 1, 1},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}},
	{{1, 0, 0, 0},
	 {1, 0, 0, 0},
	 {1, 0, 0, 0},
	 {1, 0, 0, 0}},
	{{1, 1, 1, 1},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}},
	{{1, 0, 0, 0},
	 {1, 0, 0, 0},
	 {1, 0, 0, 0},
	 {1, 0, 0, 0}}};
      memcpy (itsBitMap, tmp, sizeof (itsBitMap));  //      itsBitMap = tmp;
    }
};

class L_Stone : public Stone
{
public:
  L_Stone (): Stone (SA_6)
    {
      bool tmp[4][N_ROWS][N_COLS] = {
	{{1, 0, 0, 0},
	 {1, 0, 0, 0},
	 {1, 1, 0, 0},
	 {0, 0, 0, 0}},
	{{1, 1, 1, 0},
	 {1, 0, 0, 0},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}},
	{{1, 1, 0, 0},
	 {0, 1, 0, 0},
	 {0, 1, 0, 0},
	 {0, 0, 0, 0}},
	{{0, 0, 1, 0},
	 {1, 1, 1, 0},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}}};

      memcpy (itsBitMap, tmp, sizeof (itsBitMap));  // itsBitMap = tmp;
    }
};

class J_Stone : public Stone
{
public:
  J_Stone (): Stone (SA_1)
    {
      bool tmp[4][N_ROWS][N_COLS] = {
	{{0, 1, 0, 0},
	 {0, 1, 0, 0},
	 {1, 1, 0, 0},
	 {0, 0, 0, 0}},
	{{1, 0, 0, 0},
	 {1, 1, 1, 0},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}},
	{{1, 1, 0, 0},
	 {1, 0, 0, 0},
	 {1, 0, 0, 0},
	 {0, 0, 0, 0}},
	{{1, 1, 1, 0},
	 {0, 0, 1, 0},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}}};

      memcpy (itsBitMap, tmp, sizeof (itsBitMap));  // itsBitMap = tmp;
    }
};

class S_Stone : public Stone
{
public:
  S_Stone (): Stone (SA_2)
    {
      bool tmp[4][N_ROWS][N_COLS] = {
	{{0, 1, 1, 0},
	 {1, 1, 0, 0},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}},
	{{1, 0, 0, 0},
	 {1, 1, 0, 0},
	 {0, 1, 0, 0},
	 {0, 0, 0, 0}},
	{{0, 1, 1, 0},
	 {1, 1, 0, 0},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}},
	{{1, 0, 0, 0},
	 {1, 1, 0, 0},
	 {0, 1, 0, 0},
	 {0, 0, 0, 0}}};

      memcpy (itsBitMap, tmp, sizeof (itsBitMap));  //itsBitMap = tmp;
    }
};

class Z_Stone : public Stone
{
public:
  Z_Stone (): Stone (SA_7)
    {
      bool tmp[4][N_ROWS][N_COLS] = {
	{{1, 1, 0, 0},
	 {0, 1, 1, 0},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}},
	{{0, 1, 0, 0},
	 {1, 1, 0, 0},
	 {1, 0, 0, 0},
	 {0, 0, 0, 0}},
	{{1, 1, 0, 0},
	 {0, 1, 1, 0},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}},
	{{0, 1, 0, 0},
	 {1, 1, 0, 0},
	 {1, 0, 0, 0},
	 {0, 0, 0, 0}}};

      memcpy (itsBitMap, tmp, sizeof (itsBitMap));  //itsBitMap = tmp;
    }
};



#endif
