#include "gui_xt.hh"
#include "algorithm"

#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xutil.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>

#include <X11/Xaw/Command.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/Simple.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Text.h>
#include <X11/Xaw/Toggle.h>
#include <X11/Xaw/Scrollbar.h>
#include <X11/Xaw/Label.h>


#define MAX_CVALUE 65535	/* per X11 definition */
enum {XPIX=16, YPIX=16};

// class wide colors 
#define MAX_PIXELS 20
static unsigned long my_pixels[MAX_PIXELS];
static int my_nmb_pixels;

struct my_color
{
  const char *color_name;
  GC gc;
#define NMB_PALETTES 2
};

static struct my_color my_sa_palette[] = {
  {"Gray95", 0},
  {"Gold", 0},
  {"MediumAquamarine", 0},
  {"ForestGreen", 0},
  {"MediumOrchid", 0},
  {"OrangeRed", 0},
  {"CornFlowerBlue", 0},
  {"Coral", 0},
  {0,}};

static struct my_color my_def_palette[] = {
  {"Gray95", 0},
  {"Gray25", 0},
  {0,}};

static my_color *my_palette[] = {
  my_sa_palette, my_def_palette
};

#define SA_TO_GC(SA) (my_palette[0][SA].gc)
#define BG_GC (my_palette[1][0].gc)
#define FG_GC (my_palette[1][1].gc)



inline Display *
Xt_Play_Field_View::dpy() const
{
   return XtDisplay (itsPlayWid);
   //return XtDisplay (itsTopWidget);
}
inline int
Xt_Play_Field_View::scr_nmb() const
{
  return DefaultScreen (dpy ());
}

bool
Xt_Play_Field_View::make_palette ()
{
  if (my_nmb_pixels)
    return true; //XXX-bw/25-Jan-99

  for (unsigned i=0; i < NMB_PALETTES; ++i)
    for (unsigned ii=0; my_palette[i][ii].color_name; ++ii)
      {
	XColor xcol, exact_def_return;

	if (!XLookupColor (dpy(), DefaultColormap (dpy(), scr_nmb()),
			   my_palette[i][ii].color_name,
			   &exact_def_return, &xcol))
	  return false;
	XAllocColor (dpy(), DefaultColormap (dpy(), scr_nmb()), &xcol);
	my_palette[i][ii].gc = XCreateGC (dpy(), RootWindow (dpy(), scr_nmb()), 0, 0);
	XSetForeground (dpy(), my_palette[i][ii].gc, xcol.pixel);
	my_pixels[my_nmb_pixels++] = xcol.pixel;
    }
  return true;
}

void
Xt_Play_Field_View::destroy_palette ()
{
  XFreeColors (dpy(), DefaultColormap (dpy(), scr_nmb()),
	       my_pixels, my_nmb_pixels, 0); // ???-bw/25-Jan-99
  my_nmb_pixels = 0;
}

/* Draw a closing curtain to cover playfield after games is ending */
void
Xt_Play_Field_View::draw_curtain (bool defer)
{
  const int h = height / itsRows;

  for (unsigned row=0; row < itsRows; ++row)
    {
      for (unsigned col=0; col < itsCols; ++col)
	{
	  XFillRectangle (dpy(), XtWindow (itsPlayWid), BG_GC,
			  0, row * h,
			  width, h);
	  XFillRectangle (dpy(), XtWindow (itsPlayWid), FG_GC,
			  0, (row+1) * h,
			  width, 2);
	  if (!defer)
	    draw_flush ();
	}
      if (!defer)
	usleep (100000);
    }
}

void
Xt_Play_Field_View::draw_flush ()
{
  XSync (dpy(), False);
  //XFlush (dpy);
}

void
Xt_Play_Field_View::draw_clear (bool defer)
{
  XFillRectangle (dpy(), XtWindow (itsPlayWid), BG_GC, 0, 0, width, height);
  if (!defer)
    draw_flush ();
}

Xt_Play_Field_View::~Xt_Play_Field_View ()
{
  XtUnmapWidget (itsTopWidget); // closes window quick
  XSync (dpy(), False);
  XtUnrealizeWidget (itsTopWidget);
  XtDestroyWidget (itsTopWidget); // ???-bw/19-Jan-99
  XtDestroyApplicationContext (itsAppContext);
}


/* Callback routine when button is clicked */
/* ARGSUSED */
void
Stop(Widget w, XtPointer a, XtPointer b)
{
  exit(0);
}

void
CB_KeyPress (Widget widget, XtPointer closure, XEvent* event,
	     Boolean *continue_to_dispatch)
{
  //printf ("key press event\n");
  continue_to_dispatch = False;
}

#if 0
Xt_Play_Field_View::Xt_Play_Field_View (Play_Field *model)
  : Play_Field_View (model)
{

}
#endif

Widget
Xt_Play_Field_View::create_play_widget (Widget container)
{

  return itsPlayWid = XtVaCreateManagedWidget
    ("viewport", formWidgetClass, container,
     XtNheight, height,
     XtNwidth, width,
     0);
}

Widget
Xt_Play_Field_View::create_preview_widget (Widget container)
{
  return itsPreview =  XtVaCreateManagedWidget
    ("preview", simpleWidgetClass, container,
     XtNheight, PREV_HEIGHT,
     XtNwidth, PREV_WIDTH,
     0);
}



Xt_Play_Field_View::Xt_Play_Field_View (Play_Field *model,
					int *argc_ret,
					char *argv[])
  : Play_Field_View (model)
  , itsRows (model->get_height ())
  , itsCols (model->get_width ())
  , width (XPIX * itsCols + 1)
  , height (YPIX * itsRows + 1)
  , itsDamageMap (new Stone_Atom [itsRows * itsCols])
{
#define MW XtVaCreateManagedWidget

  //bzero (palette, sizeof palette);
  for (unsigned i=0; i < itsRows * itsCols; ++i)
    itsDamageMap[i] = SA_BORDER; // force refresh

#if 0
  itsTopWidget = XtVaAppInitialize (&itsAppContext, "Tetris", NULL, 0,
				    argc_ret, argv, NULL, NULL);
#else
  itsTopWidget = XtVaOpenApplication (&itsAppContext, "Tetris", NULL, 0,
				      argc_ret, argv, NULL,
				      applicationShellWidgetClass,
				      // XtNminWidth, 200,
				      NULL);
#endif
  // top.box
   Widget box =  MW ("box", boxWidgetClass, itsTopWidget,
		     XtNorientation, XtorientHorizontal,
		     0);

  // top.box.cbox
  Widget cbox =  MW ("cbox", boxWidgetClass, box,
					  XtNorientation, XtorientVertical,
					  0);
  // top.box.cbox.preview
  Widget pbox =  MW ("pbox", boxWidgetClass, cbox,
		     XtNorientation, XtorientVertical,
		     // XtNbackground, XtDefaultForeground,
		     0);
  (void)MW ("prev_label", labelWidgetClass, pbox, XtNlabel, "Preview", NULL);
  itsPreview =  MW ("preview", simpleWidgetClass, pbox,
		    XtNheight, PREV_HEIGHT,
		    XtNwidth, PREV_WIDTH,
		    0);
  // top.???.speed

  Widget sbox =  MW ("sbox", boxWidgetClass, cbox,
		     XtNorientation, XtorientVertical,
		     // XtNborder , XtDefaultBackground, // invisible
		     // XtNbackground, XtDefaultForeground,
		     0);
  // FIXME-bw/21-Jan-99: A scroller may not usable to choose a
  // discrete difficulty (needed at least by hiscore lists)
  // NOTE-bw/21-Jan-99: A hiscore list is not important (but boring)!
  /*Widget speed_label =*/ MW ("speed_label", labelWidgetClass, sbox, XtNlabel, "Speed", NULL);
  itsScrollerSpeed = MW ("speed", scrollbarWidgetClass, sbox,
			 XtNwidth, 10,
			 XtNheight, 80,
			 //XtNfromHoriz, speed_label,
			 NULL);
  //(void)MW ("speed_label2", labelWidgetClass, bbox, XtNlabel, "Fast", NULL);


  Widget bbox =  MW ("bbox", boxWidgetClass, cbox,
		     XtNorientation, XtorientVertical,
		     0);
  // top.box.bbox.button
  Widget button = MW ("Quit", commandWidgetClass, bbox, NULL);
  XtAddCallback(button, XtNcallback, Stop, NULL);
  // top.box.bbox.button2
  itsButtonStart = MW ("Start", commandWidgetClass, bbox, NULL);
  // top.box.bbox.button3
  itsButtonPause = MW ("Pause", toggleWidgetClass, bbox, NULL);


#if 0
  // top.viewport
  itsPlayWid = MW ("viewport", simpleWidgetClass, box,
					 XtNheight, height,
					 XtNwidth, width,
					 0);
#else
  // top.viewport
  itsPlayWid = MW ("viewport", formWidgetClass, box,
					 XtNheight, height,
					 XtNwidth, width,
					 0);
#endif
  //XtAddEventHandler (itsPlayWid, KeyPressMask, False, CB_KeyPress, 0);

  XtRealizeWidget (itsTopWidget);

  // We want the input-focus to use keyboard (We must do this ourself
  // because we don't use a widget needing keyboard) -bw/20-Jan-99
  if (XWMHints *hints = XAllocWMHints())
    {
      hints->flags = InputHint;
      hints->input = True;
      XSetWMHints (dpy(), XtWindow (itsTopWidget), hints);
      XFree (hints);
    }
  {
    XSetWindowAttributes wa;
    wa.event_mask = (ExposureMask | KeyPressMask | KeyReleaseMask);
    XChangeWindowAttributes (dpy(), XtWindow (itsTopWidget), CWEventMask, &wa);
    wa.event_mask = (ExposureMask);
    XChangeWindowAttributes (dpy(), XtWindow (itsPlayWid), CWEventMask, &wa);
    XChangeWindowAttributes (dpy(), XtWindow (itsPreview), CWEventMask, &wa);
  }

  if (!make_palette ()) throw;
  draw_clear ();
#undef MW
}

void
Xt_Play_Field_View::draw_pixel (unsigned row, unsigned column, GC color,
				bool defer)
{
  const int h = height / itsRows;
  const int w = width / itsCols;

  XFillRectangle (dpy(), XtWindow (itsPlayWid), color, column * w, row * h, w, h);
  XDrawRectangle (dpy(), XtWindow (itsPlayWid), FG_GC, column * w, row * h, w, h);
  if (!defer)
    draw_flush ();
}

void
Xt_Play_Field_View::dump_pixel_rectangle (unsigned pix_x, unsigned pix_y,
					  unsigned pix_width, unsigned pix_height)
{
  Play_Field *pf = itsModel;
  if (!pf)
    return;

  unsigned x = max ((int)pix_x / XPIX - 1, 0);
  unsigned y = max ((int)pix_y / YPIX - 1, 0);
  unsigned width = min (pix_width / XPIX + 2, itsCols - x);
  unsigned height = min (pix_height / YPIX + 2, itsRows - y);
  dump_rectangle (x, y, width, height);
}

void
Xt_Play_Field_View::dump_rectangle (unsigned x, unsigned y,
				   unsigned width, unsigned height)
{
  Play_Field *pf = itsModel;
  if (!pf)
    return;
  for (unsigned row=y; row < y+height; ++row)
    for (unsigned col=x; col < x+width; ++col)
      {
	Stone_Atom sa = pf->get_field (row, col);
	set_damage (row, col, sa);
	draw_pixel (row, col, SA_TO_GC (sa), true);
      }
  draw_flush ();
}

void
Xt_Play_Field_View::update ()
{
  Play_Field *pf = itsModel;
  if (!pf)
    return;
  for (unsigned row=0; row < itsRows; ++row)
    for (unsigned col=0; col < itsCols; ++col)
      {
	Stone_Atom sa = pf->get_field (row, col);
	if (test_damage (row, col, sa))
	  {
	    set_damage (row, col, sa);
	    draw_pixel (row, col, SA_TO_GC (sa), true);
	  }
      }
  draw_flush ();
}

void
Xt_Play_Field_View::update_preview (Stone &stone)
{
  for (unsigned row=0; row < 4; ++row)
    for (unsigned col=0; col < 4; ++col)
      {
	GC color = (stone.test_pixel (row, col, Stone::SAL_NORMAL)
			  ? SA_TO_GC (stone.get_atom ())
			  : SA_TO_GC (SA_0));
	const int h = PREV_HEIGHT / 4;
	const int w = PREV_WIDTH / 4;

	XFillRectangle (dpy(), XtWindow (itsPreview), color, col * w, row * h, w, h);
	XDrawRectangle (dpy(), XtWindow (itsPreview), FG_GC, col * w, row * h, w, h);
      }
  draw_flush ();
}


#include <X11/keysym.h>

void
Xt_Play_Field_Control::CB_speed_scroll (Widget w, XtPointer a, XtPointer b)
{
  //  Xt_Play_Field_Control *obj = (Xt_Play_Field_Control *) a;
}

void
Xt_Play_Field_Control::CB_speed_jump (Widget w, XtPointer a, XtPointer call_data)
{
  Xt_Play_Field_Control *obj = (Xt_Play_Field_Control *) a;
  obj->itsTimerIntervalDefault 
    = obj->itsTimerInterval 
    = (long)TI_SLOW - (long)(((long)TI_SLOW - (long)TI_FAST) * (*(float*)call_data));
}

void
Xt_Play_Field_Control::CB_toggle_pause (Widget w, XtPointer a, XtPointer b)
{
  Xt_Play_Field_Control *obj = (Xt_Play_Field_Control *) a;

  obj->isPaused = !obj->isPaused;

  if (!obj->isRunning)
    return;

  if (obj->isPaused)
    {
      if (obj->isTimerRunning)
	{
	  XtRemoveTimeOut (obj->itsTimer);
	  obj->isTimerRunning = false;
	}
    }
  else
    {
      if (!obj->isTimerRunning)

	{	obj->itsTimer = XtAppAddTimeOut (obj->itsView.itsAppContext,
						 obj->itsTimerInterval,
						 CB_timeout, (XtPointer)obj);
	obj->isTimerRunning = true;
	}
    }
}

void
Xt_Play_Field_Control::CB_start (Widget w, XtPointer a, XtPointer b)
{
  Xt_Play_Field_Control *obj = (Xt_Play_Field_Control *) a;

  if (obj->isRunning)
    return;
  obj->isRunning = true;

  if (!obj->isPaused && !obj->isTimerRunning)
    {
      obj->itsTimer = XtAppAddTimeOut (obj->itsView.itsAppContext,
				       obj->itsTimerInterval,
				       CB_timeout, (XtPointer)obj);
      obj->isTimerRunning = true;
    }
}

void
Xt_Play_Field_Control::CB_timeout (XtPointer a, XtIntervalId *id)
{
#define field (*obj->itsView.itsModel)
  Xt_Play_Field_Control *obj = (Xt_Play_Field_Control *) a;
  bool display_changed=false;
  bool stop = false;

  if  (field.test_collision ())
    {
      // remove full rows
      for (unsigned row=0; row < field.get_height (); ++row)
	{
	  for (unsigned col=0; col < field.get_width (); ++col)
	    if (!field.get_field (row, col))
	      goto next_row;
	  field.remove_row (row);
	  display_changed = true;
	next_row:;
	}

      // make new current stone
      Stone &stone = *obj->stone_set[obj->itsNextStone];
      display_changed = true;
      field.set_current_stone (stone);

      obj->itsNextStone = random () % obj->nmb_stones;
      obj->itsView.update_preview (*obj->stone_set[obj->itsNextStone]);

      if  (field.test_collision ())
	stop = true;
      obj->itsTimerInterval = obj->itsTimerIntervalDefault;
    }
  else
    {
      if (field.move_stone (DIR_SOUTH))
	display_changed = true;
    }

  if (stop)
    obj->isTimerRunning = false;
  else
    obj->itsTimer = XtAppAddTimeOut (obj->itsView.itsAppContext,
				     obj->itsTimerInterval,
				     (XtTimerCallbackProc) CB_timeout,
				     (XtPointer)obj);
  if (display_changed)
    field.notify_views ();
#undef field
}

void
Xt_Play_Field_Control::process_events (bool block)
{

  while (1)
    {
      if (block)
	{
	}
      else if (! XtAppPending (itsView.itsAppContext))
	break;

      XEvent e;
      XtAppNextEvent (itsView.itsAppContext, &e);
      switch(e.type)
	{
	case KeyPress:
	  switch (e.xkey.keycode)
	    {
	    case 100: //XK_Left:
	      itsView.itsModel->move_stone (DIR_WEST);
	      itsView.itsModel->notify_views ();
	      goto parent;
	    case 102: //XK_Right:
	      itsView.itsModel->move_stone (DIR_EAST);
	      itsView.itsModel->notify_views ();
	      goto parent;
	    case 98: // XK_Up:
	      itsView.itsModel->rotate_stone_90_cw ();
	      itsView.itsModel->notify_views ();
	      goto parent;
	    case 104: // XK_Down:
	      if (isTimerRunning)
		{
		  XtRemoveTimeOut (itsTimer);
		  itsTimerInterval = TI_FALL;
		  CB_timeout (this, 0);
		}
	      goto parent;
	    default:
//	      fprintf (stderr, "keypress: %d\n", e.xkey.keycode);
	      goto parent;
	    }
	  break;

	    case KeyRelease:
	      switch (e.xkey.keycode)
		{
		case 104: // XK_Down:
		  if (isTimerRunning)
		    {
		      XtRemoveTimeOut (itsTimer);
		      itsTimerInterval = itsTimerIntervalDefault;
		      CB_timeout (this, 0);
		    }
		  goto parent;
		default:
		  //fprintf (stderr, "keypress: %d\n", e.xkey.keycode);
		  goto parent;
		}
	      break;

	case Expose:
	  if (e.xexpose.window == XtWindow (itsView.itsPlayWid))
	    {
	      itsView.dump_pixel_rectangle (e.xexpose.x, e.xexpose.y,
					    e.xexpose.width, e.xexpose.height);
	      //itsView.dump_rectangle (0, 0, itsView.itsCols, itsView.itsRows);
	    }
	  else if (e.xexpose.count)
	    goto parent;
	  if (e.xexpose.window == XtWindow (itsView.itsPreview))
	    itsView.update_preview (*stone_set[itsNextStone]);
	  // TODO-bw/18-Jan-99
	  goto parent;


	default:
	parent:
	  //	  printf ("event\n");
	  XtDispatchEvent (&e);
	}
    } 
  if (isTimerRunning)
    XtRemoveTimeOut (itsTimer);
}


Xt_Play_Field_Control::Xt_Play_Field_Control (Xt_Play_Field_View &view,
					      Stone *stones[])
  : itsView (view)
  , isRunning (false), isPaused (false), isTimerRunning (false)
  , itsTimerIntervalDefault (TI_NORMAL)
  , itsTimerInterval (itsTimerIntervalDefault)
  , nmb_stones (0)
  
{
  while (stones[nmb_stones])
    ++nmb_stones;
  stone_set = new Stone *[nmb_stones];
  for (unsigned i=0; i < nmb_stones; ++i)
    stone_set[i] = stones[i];
  itsNextStone = random () % nmb_stones;

  XtAddCallback (itsView.itsButtonStart, XtNcallback, CB_start,
		 (XtPointer) this);

  //printf ("client_data %p\n", (XtPointer) this);
  XtAddCallback (itsView.itsButtonPause, XtNcallback,
		 CB_toggle_pause, (XtPointer) this);
  XtAddCallback (itsView.itsScrollerSpeed, XtNjumpProc, CB_speed_jump, (XtPointer)this);
  //  XtAddCallback (itsView.itsScrollerSpeed, XtNscrollProc, CB_speed_scroll, (XtPointer)this);
}




