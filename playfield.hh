#ifndef H_TETRIS_PLAYFIELD_HH_
#define H_TETRIS_PLAYFIELD_HH_

#include "framework.hh"

class Tetris_Play_Field : public Play_Field
{
public:
  Tetris_Play_Field (int width, int height);
  ~Tetris_Play_Field ();
  virtual bool test_collision () const;
  virtual bool test_overlap (Stone::Stone_Angle angle) const;
  // Bewege "current_stone"
  // Wenn erfolgreich result=true, andernfalls (Kollision) result=false
  virtual bool move_stone (Direction d);
  // Rotiere "current_stone" um 90 Grad im Uhrzeigersinn
  // Wenn erfolgreich result=true, andernfalls (Kollision) result=false
  virtual bool rotate_stone_90_cw ();
  // entferne Zeile N und lasse darueberliegende Zeilen nachrutschen
  virtual void remove_row (unsigned n);
  // Liefert zu zeichnendes Objekt fuer View (der View verwaltet
  // eine Map Stone_Atom=>XXX_View_Stone_Atom.
  virtual Stone_Atom get_field (int row, int column) const 
    { return itsPixMap [row * (itsWidth+2) + (column+1)]; }
    //    { return (Stone_Atom) (random () % 6); }
public: // implementing base class interface
  virtual void set_current_stone (Stone &s);
private:
  void map_current_stone ();
  void unmap_current_stone ();
  void set_field (int row, int column, Stone_Atom val) 
    { itsPixMap [row * (itsWidth+2) + (column+1)] = val; }
private:
  Stone *itsCurrStone;
  int itsXPos, itsYPos; /* positions of current stone (top-left edge) */
  Stone::Stone_Angle itsAngle;
  Stone_Atom *itsPixMap;
};


#endif
