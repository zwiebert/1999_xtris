#ifndef H_TETRIS_FRAMEWORK_H_
#define H_TETRIS_FRAMEWORK_H_

#include "mvc.hh"

class Clock
{
public:
  void set_rate (int beats_per_minute);
  void wait_for_next_beat () const;
};

enum Direction { DIR_WEST, DIR_EAST, DIR_SOUTH };
enum Stone_Atom { SA_BORDER=3, SA_BACKGROUND=0, SA_0=0,
		  SA_1, SA_2, SA_3, SA_4, SA_5, SA_6, SA_7, SA_Nmb };

template <unsigned N_Rows, unsigned N_Columns>
class StoneBitMap
{
public:
  StoneBitMap (Stone_Atom sa) : itsAtom (sa) {}
  enum Stone_Angle { SAL_NORMAL, SAL_90, SAL_180, SAL_270 };
  bool test_pixel (unsigned row, unsigned column,
		   Stone_Angle angle) const
    { return itsBitMap[angle][row][column]; }
  unsigned width () const { return N_Rows; }
  unsigned height () const { return N_Columns; }
  Stone_Atom get_atom () const { return itsAtom; }
protected:
  enum {N_ROWS = N_Rows, N_COLS = N_Columns};
  bool itsBitMap[4][N_Rows][N_Columns];
  Stone_Atom itsAtom;
};
typedef StoneBitMap <4, 4> Stone;


class Play_Field : public Model
{
public:
  Play_Field (int width, int height): itsWidth (width), itsHeight (height) {}
  // entferne Reihe (mit Nachrutschen)
  virtual void remove_row (unsigned n)=0;
  // Setze Typ fuer den aktuellen Spielstein ("current_stone")
  virtual void set_current_stone (Stone &s)=0;
  // teste ob Aufprall des "current_stone" erfolgt ist;
  virtual bool test_collision () const { return true; }
  // Bewege "current_stone"
  // Wenn erfolgreich result=true
  virtual bool move_stone (Direction d)=0;
  // Rotiere "current_stone" um 90 Grad im Uhrzeigersinn
  // Wenn erfolgreich result=true, andernfalls (Kollision) result=false
  virtual bool rotate_stone_90_cw ()=0;
  // Liefert zu zeichnendes Objekt fuer View (der View verwaltet
  // eine Map Stone_Atom=>XXX_View_Stone_Atom.
  virtual Stone_Atom get_field (int row, int column) const=0;
  unsigned get_width () const { return itsWidth; }
  unsigned get_height () const { return itsHeight; }
protected:
  unsigned itsWidth, itsHeight;
};


class Play_Field_Control
{
public:
  virtual ~Play_Field_Control () {};
  virtual void process_events (bool block)=0;
};

// inherited by View
// public: virtual void update ()=0;

class Play_Field_View: public View
{
public:
  ~Play_Field_View ();
  Play_Field_View (Play_Field *model): itsModel (0) { set_model (model); }
private:
  void set_model (Play_Field *model);
protected:
  Play_Field *itsModel;
};

#endif
