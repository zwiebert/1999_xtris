#ifndef H_TETRIS_GUI_X_H_
#define H_TETRIS_GUI_X_H_

#include "framework.hh"
#include <X11/Xlib.h>

class X_Play_Field_Control;
class X_Play_Field_View;

class X_Play_Field_View : public Play_Field_View
{
  friend class X_Play_Field_Control;
public:
  virtual ~X_Play_Field_View ();
  X_Play_Field_View (Play_Field *model);
  // do optimized refreshing using cache
  virtual void update ();
  // to repair exposed areas (not using cache)
  void dump_rectangle (unsigned x, unsigned y,
		       unsigned width, unsigned height);

  enum my_color { white, yellow, cyan, green, magenta,
		  red, blue, black, NMB_colors };
private:
  bool open_window ();
  bool make_palette ();
  void draw_curtain (bool defer = false);
  void draw_clear (bool defer = false);  // draw empty field
  void draw_flush ();  // 
  void draw_pixel (unsigned row, unsigned column, my_color color, bool defer = false);
  // optimized display updating
  bool test_damage (unsigned row, unsigned col, Stone_Atom val)
    { return itsDamageMap[row * itsCols + col] != val; }
  void set_damage (unsigned row, unsigned col, Stone_Atom val)
    { itsDamageMap[row * itsCols + col] = val; }
private:
  const static my_color sa2col_map[SA_Nmb];

  enum { N_COLORS=8 };
  Display *dpy;
  Screen *scr_ptr;
  Window win;
  GC palette[N_COLORS];
  int scr_nmb;
  unsigned itsRows, itsCols;
  int width, height;
  Stone_Atom *itsDamageMap;
};


class X_Play_Field_Control : public Play_Field_Control
{
public:
  X_Play_Field_Control (X_Play_Field_View &view) :
    itsView (view)
    {}
  void process_events ();
private:
  const X_Play_Field_View &itsView;
};

#endif
