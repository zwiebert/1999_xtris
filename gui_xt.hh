#ifndef H_TETRIS_GUI_XT_H_
#define H_TETRIS_GUI_XT_H_

#include "framework.hh"
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

class Xt_Play_Field_Control;
class Xt_Play_Field_View;

class Xt_Play_Field_View : public Play_Field_View
{
  friend class Xt_Play_Field_Control;
public:
  virtual ~Xt_Play_Field_View ();
  Xt_Play_Field_View (Play_Field *model, int *argc_ret, char *argv[]);

  // (optimized) window refreshing called when model has changed.
  virtual void update ();

  // window refreshing called for expose events (low level, not using cache)
  void dump_rectangle (unsigned x, unsigned y,
		       unsigned width, unsigned height);
  void dump_pixel_rectangle (unsigned pix_x, unsigned pix_y,
			     unsigned pix_width, unsigned pix_height);

  // display next stone in itsPreview widget
  enum { PREV_WIDTH = 25, PREV_HEIGHT = 25 };
  void update_preview (Stone &stone);

private:
  bool open_window ();
  bool make_palette ();
  void destroy_palette ();
  void draw_curtain (bool defer = false);
  void draw_clear (bool defer = false);  // draw empty field
  void draw_flush ();  // synch with X server
  void draw_pixel (unsigned row, unsigned column, GC color, bool defer = false);
  // optimized display updating
  bool test_damage (unsigned row, unsigned col, Stone_Atom val)
    { return itsDamageMap[row * itsCols + col] != val; }
  void set_damage (unsigned row, unsigned col, Stone_Atom val)
    { itsDamageMap[row * itsCols + col] = val; }

private:
  Display *dpy;
  //  GC palette[N_COLORS];
  int scr_nmb;
  unsigned itsRows, itsCols;
  int width, height;

  /* data store (mirrors itsPlayWid) for optimized ouput window
     refreshing */
  Stone_Atom *itsDamageMap;

  /* *** Controller section (FIXME) ***  */
  XtAppContext itsAppContext;
  /* control widgets */
  Widget itsTopWidget, itsButtonStart, itsButtonPause, itsScrollerSpeed;
  /* output window widgets */
  Widget itsPlayWid, itsPreview;
};


class Xt_Play_Field_Control : public Play_Field_Control
{
public:
  Xt_Play_Field_Control (Xt_Play_Field_View &view, Stone *stones[]);

  void process_events (bool block);
private:
  enum {TI_NORMAL = 500, TI_FAST = 100, TI_SLOW = 700, TI_FALL = 10};
  // Xt callbacks
  static void CB_start (Widget w, XtPointer a, XtPointer b);
  static void CB_toggle_pause (Widget w, XtPointer a, XtPointer b);
  static void CB_speed_scroll (Widget w, XtPointer a, XtPointer b);
  static void CB_speed_jump (Widget w, XtPointer a, XtPointer b);
  static void CB_timeout (XtPointer obj, XtIntervalId *id);
private:
  Xt_Play_Field_View &itsView;
  XtIntervalId itsTimer;
  bool isRunning, isPaused, isTimerRunning;
  long itsTimerIntervalDefault, itsTimerInterval; // milli seconds
  Stone **stone_set;
  unsigned nmb_stones;
  unsigned itsNextStone;
};

#endif
