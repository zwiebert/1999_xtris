#ifndef H_TETRIS_DUMMY_H_
#define H_TETRIS_DUMMY_H_

#include "framework.hh"

class Dummy_Play_Field : public Play_Field
{
public:
  Dummy_Play_Field (int width, int height): Play_Field (width, height) {}
  virtual void set_current_stone (Stone &s) {}
  // Bewege "current_stone"
  // Wenn erfolgreich result=true, andernfalls (Kollision) result=false
  virtual bool move_stone (Direction d) { return false; }
  // Rotiere "current_stone" um 90 Grad im Uhrzeigersinn
  // Wenn erfolgreich result=true, andernfalls (Kollision) result=false
  virtual bool rotate_stone_90_cw () { return false; }
  // Liefert zu zeichnendes Objekt fuer View (der View verwaltet
  // eine Map Stone_Atom=>XXX_View_Stone_Atom.
  virtual Stone_Atom get_field (int row, int column) const 
    { return (Stone_Atom) (random () % 6); }
};


class Dummy_Play_Field_Control : public Play_Field_Control
{
public:
  virtual void process_events (bool) {}
};


class Dummy_Play_Field_View : public Play_Field_View
{
public:
  Dummy_Play_Field_View (Play_Field *model) : Play_Field_View (model) {}
  ~Dummy_Play_Field_View () {}
  virtual void update () {}
};

#endif
