#include "mvc.hh"

class Clock
{
 public:
  void set_rate (int beats_per_minute);
  void wait_for_next_beat () const;
};

enum color_en { COL_NONE, COL_RED, COL_BLUE, COL_GREEN,
	       COL_YELLOW, COL_PURPLE, COL_CYAN };
enum Direction { DIR_WEST, DIR_EAST, DIR_SOUTH };

struct Point { int x, y; };

class Stone_Atom
{
 public:
  Stone_Atom (color_en color);
  draw ();
};

class Stone
{
 public:
  //  virtual bool rotate90cw (Rectangle free_space) = 0;
  // virtual color_T get_color () const = 0;
  virtual color_en get_color (int row, int column) const = 0;
  virtual int width () = 0;
  virtual int height () = 0;
  virtual Point center () = 0;
};



   
class Play_Field : public Model
{
public:
  Play_Field (int width, int height);
  // Setze Typ fuer den aktuellen Spielstein ("current_stone")
  void set_current_stone (Stone &s);
  // Bewege "current_stone"
  // Wenn erfolgreich result=true, andernfalls (Kollision) result=false
  bool move_stone (Direction d);
  // Rotiere "current_stone" um 90 Grad im Uhrzeigersinn
  // Wenn erfolgreich result=true, andernfalls (Kollision) result=false
  bool rotate_stone_90_cw ();
  color_en get_color (int row, int column) const;
};


class Play_Field_Control
{
public:
  virtual void process_events ()=0;
};


class Play_Field_View : public View
{
public:
  ~Play_Field_View () { if (itsModel) itsModel->detach_view (this); }
  void update ();
  void set_model (Play_Field *model)
    {
      if (itsModel) itsModel->detach_view (this);
      itsModel = model;
      if (itsModel)
	{
	  itsModel->attach_view (this);
	  update ();
	}
    }
private:
  Play_Field *itsModel;
};

