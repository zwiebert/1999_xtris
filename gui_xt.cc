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
  {"Gray90", 0},
  {"Gold", 0},
  {"MediumAquamarine", 0},
  {"ForestGreen", 0},
  {"MediumOrchid", 0},
  {"OrangeRed", 0},
  {"CornFlowerBlue", 0},
  {"Coral", 0},
  {0,}};

static struct my_color my_def_palette[] = {
  {"Gray25", 0},
  {"Gray35", 0}, // 3D SHADOW
  {"Gray95", 0}, // 3D LIGHT
  {0,}};

static my_color *my_palette[] = {
  my_sa_palette, my_def_palette
};

#define SA_TO_GC(SA) (my_palette[0][SA].gc)
#define BG_GC (my_palette[0][0].gc)
#define FG_GC (my_palette[1][0].gc)
#define SHADOW_EDGE_GC (my_palette[1][1].gc)
#define LIGHT_EDGE_GC (my_palette[1][2].gc)


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

bool
Xt_Play_Field_View::init_after_realize_widgets ()
{
  if (make_palette ())
    {
      return true;
    }
  return false;
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
  destroy_palette ();
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
  continue_to_dispatch = False;
}

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



Xt_Play_Field_View::Xt_Play_Field_View (Play_Field *model)
  : Play_Field_View (model)
  , itsLook (&draw_pixel_rectangle)
  , itsRows (model->get_height ())
  , itsCols (model->get_width ())
  , width (XPIX * itsCols + 1)
  , height (YPIX * itsRows + 1)
  , itsDamageMap (new Stone_Atom [itsRows * itsCols])
  , itsPlayWid (0), itsPreview (0), itsNextStone (0)
{
  //bzero (palette, sizeof palette);
  for (unsigned i=0; i < itsRows * itsCols; ++i)
    itsDamageMap[i] = SA_BORDER; // force refresh

}


void
Xt_Play_Field_View::draw_pixel_triangle (unsigned row, unsigned column, GC color)
{
  const int h = (height / itsRows);
  const int w = (width / itsCols);
  const int x = column * w;
  const int y = row * h;

  XPoint pts[] = {{x,y}, {x+w-2,y}, {x+w-2,y+h-2,}, {x,y}};
  XFillPolygon (dpy(), XtWindow (itsPlayWid), color,
		pts, 3, Convex, CoordModeOrigin);
  if (color == SA_TO_GC (SA_BACKGROUND))
    XDrawLines (dpy(), XtWindow (itsPlayWid), BG_GC,
		pts, 4, CoordModeOrigin);
  else
    {
      XDrawLines (dpy(), XtWindow (itsPlayWid), LIGHT_EDGE_GC,
		pts, 1, CoordModeOrigin);
      XDrawLines (dpy(), XtWindow (itsPlayWid), SHADOW_EDGE_GC,
		  &pts[1], 3, CoordModeOrigin);
    }
}


void
Xt_Play_Field_View::draw_pixel_circle (unsigned row, unsigned column, GC color)
{
  const int h = (height / itsRows);
  const int w = (width / itsCols);
  const int x = column * w;
  const int y = row * h;

  XFillArc(dpy(), XtWindow (itsPlayWid), color,
	   x, y, w-2, h-2,
	   0 * 64, 360 * 64);
  if (color == SA_TO_GC (SA_BACKGROUND))
    XDrawArc(dpy(), XtWindow (itsPlayWid), BG_GC,
	     x, y, w-2, h-2,
	     0 * 64, 360 * 64);
  else
    {
      XDrawArc(dpy(), XtWindow (itsPlayWid), LIGHT_EDGE_GC,
	       x, y, w-2, h-2,
	       90 * 64, 90 * 64);
      XDrawArc(dpy(), XtWindow (itsPlayWid), SHADOW_EDGE_GC,
	       x, y, w-2, h-2,
	       180 * 64, 270 * 64);
    }
}

void
Xt_Play_Field_View::draw_pixel_rectangle (unsigned row, unsigned column, GC color)
{
  const int h = (height / itsRows);
  const int w = (width / itsCols);
  const int x = column * w;
  const int y = row * h;

  XFillRectangle (dpy(), XtWindow (itsPlayWid), color,
		  column * w, row * h, w-1, h-1);

  if (color == SA_TO_GC (SA_BACKGROUND))
    XDrawRectangle (dpy(), XtWindow (itsPlayWid), BG_GC,
		    column * w, row * h, w-1, h-1);
  else
    {
      XPoint pts[] = {{x,y+h-1,}, {x,y}, {x+w-1,y},
		      {x+w-1,y+h-1,}, {x,y+h-1,}};
      XDrawLines (dpy(), XtWindow (itsPlayWid), LIGHT_EDGE_GC,
		  &pts[0], 3, CoordModeOrigin);
      XDrawLines (dpy(), XtWindow (itsPlayWid), SHADOW_EDGE_GC,
		  &pts[2], 3, CoordModeOrigin);
    }
}

void
Xt_Play_Field_View::choose_look (void (self::*method) (unsigned row, unsigned column, GC color))
{
  itsLook = method;
  dump_pixel_rectangle (0, 0, width, height);
}

void
Xt_Play_Field_View::draw_pixel_plain (unsigned row, unsigned column, GC color)
{
  const int h = (height / itsRows);
  const int w = (width / itsCols);

  XFillRectangle (dpy(), XtWindow (itsPlayWid), color,
		  column * w, row * h, w, h);
  XDrawRectangle (dpy(), XtWindow (itsPlayWid), FG_GC,
		  column * w, row * h, w, h);
}

void
Xt_Play_Field_View::draw_pixel (unsigned row, unsigned column, GC color,
				bool defer)
{
  if (!itsLook)
    throw;

  (this->*itsLook) (row, column, color);
  if (!defer)
    draw_flush ();
}

void
Xt_Play_Field_View::dump_pixel_rectangle (unsigned pix_x, unsigned pix_y,
					  unsigned pix_width,
					  unsigned pix_height)
{
  Play_Field *pf = itsModel;
  if (!pf)
    return;
  fprintf (stderr, "dump_pix_rect: %u %u %u %u\n", pix_x, pix_y, pix_width, pix_height);

  XFillRectangle (dpy(), XtWindow (itsPlayWid), BG_GC, 
		  pix_x, pix_y, pix_width, pix_height);

  unsigned x = max ((int)pix_x / XPIX - 1, 0);
  unsigned y = max ((int)pix_y / YPIX - 1, 0);
  unsigned width = min (pix_width / XPIX + 2, itsCols - x);
  unsigned height = min (pix_height / YPIX + 2, itsRows - y);
  fprintf (stderr, "dump_rect: %u %u %u %u\n", x, y, width, height);
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
Xt_Play_Field_View::update_preview ()
{
  for (unsigned row=0; row < 4; ++row)
    for (unsigned col=0; col < 4; ++col)
      {
	GC color = (itsNextStone->test_pixel (row, col, Stone::SAL_NORMAL)
			  ? SA_TO_GC (itsNextStone->get_atom ())
			  : SA_TO_GC (SA_0));
	const int h = PREV_HEIGHT / 4;
	const int w = PREV_WIDTH / 4;

	XFillRectangle (dpy(), XtWindow (itsPreview), color,
			col * w, row * h, w, h);
	XDrawRectangle (dpy(), XtWindow (itsPreview), FG_GC,
			col * w, row * h, w, h);
      }
  draw_flush ();
}

void
Xt_Play_Field_View::expose_play_widget (XExposeEvent &e)
{
  dump_pixel_rectangle (e.x, e.y, e.width, e.height);
}

void
Xt_Play_Field_View::expose_preview_widget (XExposeEvent &e)
{
  if (!e.count)
    update_preview ();
}



#include <X11/keysym.h>

inline Display *
Xt_Play_Field_Control::dpy() const
{
  return XtDisplay (itsTopWidget);
}
inline int
Xt_Play_Field_Control::scr_nmb() const
{
  return DefaultScreen (dpy ());
}

void
Xt_Play_Field_Control::CB_speed_scroll (Widget w, XtPointer a, XtPointer b)
{
  //  Xt_Play_Field_Control *obj = (Xt_Play_Field_Control *) a;
}

void
Xt_Play_Field_Control::cb_speed_jump (const float &pos)
{
  itsTimerIntervalDefault
    = itsTimerInterval 
    = (long)TI_SLOW - (long)(((long)TI_SLOW - (long)TI_FAST) * pos);
}

void
Xt_Play_Field_Control::CB_speed_jump (Widget w, XtPointer a, XtPointer b)
{
  ((self *) a)->cb_speed_jump (*(float*)b);
}

void
Xt_Play_Field_Control::cb_toggle_pause ()
{
  isPaused = !isPaused;

  if (!isRunning)
    return;

  if (isPaused)
    {
      if (isTimerRunning)
	{
	  XtRemoveTimeOut (itsTimer);
	  isTimerRunning = false;
	}
    }
  else
    {
      if (!isTimerRunning)

	{	itsTimer = XtAppAddTimeOut (itsAppContext,
					    itsTimerInterval,
					    CB_timeout, (XtPointer)this);
	isTimerRunning = true;
	}
    }
}

void
Xt_Play_Field_Control::CB_toggle_pause (Widget w, XtPointer a, XtPointer b)
{
  ((self *) a)->cb_toggle_pause ();
}


void
Xt_Play_Field_Control::cb_toggle_look ()
{
  switch (++itsLookID %= 4)
    {
    case 0:
      itsView.choose_look (&itsView.draw_pixel_rectangle);
      break;
    case 1:
      itsView.choose_look (&itsView.draw_pixel_circle);
      break;
    case 2:
      itsView.choose_look (&itsView.draw_pixel_triangle);
      break;
    case 3:
      itsView.choose_look (&itsView.draw_pixel_plain);
      break;
    }
}

void
Xt_Play_Field_Control::CB_toggle_look (Widget w, XtPointer a, XtPointer b)
{
  ((self *) a)->cb_toggle_look ();
}

void
Xt_Play_Field_Control::cb_start ()
{
  if (isRunning)
    return;
  isRunning = true;

  if (!isPaused && !isTimerRunning)
    {
      itsTimer = XtAppAddTimeOut (itsAppContext,
				  itsTimerInterval,
				  CB_timeout, (XtPointer)this);
      isTimerRunning = true;
    }
}

void
Xt_Play_Field_Control::CB_start (Widget w, XtPointer a, XtPointer b)
{
  ((self *) a)->cb_start ();
}

void
Xt_Play_Field_Control::cb_timeout ()
{
  bool display_changed=false;
  bool stop = false;

  if  (itsModel.test_collision ())
    {
      // remove full rows
      for (unsigned row=0; row < itsModel.get_height (); ++row)
	{
	  for (unsigned col=0; col < itsModel.get_width (); ++col)
	    if (!itsModel.get_field (row, col))
	      goto next_row;
	  itsModel.remove_row (row);
	  display_changed = true;
	next_row:;
	}

      // make new current stone
      Stone &stone = *stone_set[itsNextStone];
      display_changed = true;
      itsModel.set_current_stone (stone);

      itsNextStone = random () % nmb_stones;
      itsView.set_preview_stone (*stone_set[itsNextStone]);
      itsView.update_preview ();

      if  (itsModel.test_collision ())
	stop = true;
      itsTimerInterval = itsTimerIntervalDefault;
    }
  else
    {
      if (itsModel.move_stone (DIR_SOUTH))
	display_changed = true;
    }

  if (stop)
    isTimerRunning = false;
  else
    itsTimer = XtAppAddTimeOut (itsAppContext,
				itsTimerInterval,
				(XtTimerCallbackProc) CB_timeout,
				(XtPointer)this);
  if (display_changed)
    itsModel.notify_views ();
}

void
Xt_Play_Field_Control::CB_timeout (XtPointer a, XtIntervalId *id)
{
  ((self *) a)->cb_timeout ();
}

void
Xt_Play_Field_Control::process_events (bool block)
{

  while (1)
    {
      if (!block && !XtAppPending (itsAppContext))
	break;

      XEvent e;
      XtAppNextEvent (itsAppContext, &e);
      switch (e.type)
	{
	case KeyPress:
	  switch (e.xkey.keycode)
	    {
	    case 100: //XK_Left:
	      itsModel.move_stone (DIR_WEST);
	      itsModel.notify_views ();
	      goto parent;
	    case 102: //XK_Right:
	      itsModel.move_stone (DIR_EAST);
	      itsModel.notify_views ();
	      goto parent;
	    case 98: // XK_Up:
	      itsModel.rotate_stone_90_cw ();
	      itsModel.notify_views ();
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
		  goto parent;
		}
	      break;

#if 0
	case MapNotify:
	  fprintf (stderr, "MapNotify\n");
	  itsView.draw_clear ();
	  break;
#endif

	case Expose:
	  if (e.xexpose.window == XtWindow (itsPlayWid))
	    itsView.expose_play_widget (e.xexpose);

	  if (e.xexpose.window == XtWindow (itsPreview))
	    itsView.expose_preview_widget (e.xexpose);
	  
	  goto parent;


	default:
	parent:
	  XtDispatchEvent (&e);
	}
    } 
  if (isTimerRunning)
    XtRemoveTimeOut (itsTimer);
}

Xt_Play_Field_Control::~Xt_Play_Field_Control ()
{
  XtUnmapWidget (itsTopWidget); // closes window quick
  XSync (dpy(), False);
  XtUnrealizeWidget (itsTopWidget);
  XtDestroyWidget (itsTopWidget); // ???-bw/19-Jan-99
  XtDestroyApplicationContext (itsAppContext);
}


Xt_Play_Field_Control::Xt_Play_Field_Control (Play_Field &model,
					      Xt_Play_Field_View &view,
					      Stone *stones[],
					      int *argc_ret, char *argv[])
  : itsModel (model), itsView (view)
  , isRunning (false), isPaused (false), isTimerRunning (false)
  , itsTimerIntervalDefault (TI_NORMAL)
  , itsTimerInterval (itsTimerIntervalDefault)
  , nmb_stones (0)
  , itsLookID (0)
{
  while (stones[nmb_stones])
    ++nmb_stones;
  stone_set = new Stone *[nmb_stones];
  for (unsigned i=0; i < nmb_stones; ++i)
    stone_set[i] = stones[i];
  itsNextStone = random () % nmb_stones;
  itsView.set_preview_stone (*stone_set[itsNextStone]);

  // build widget tree
  {
#define MW XtVaCreateManagedWidget

    itsTopWidget = XtVaOpenApplication (&itsAppContext, "Tetris", NULL, 0,
					argc_ret, argv, NULL,
					applicationShellWidgetClass,
					// XtNminWidth, 200,
					NULL);

    // top.box
    Widget box = MW ("box", boxWidgetClass, itsTopWidget,
		     XtNorientation, XtorientHorizontal,
		     0);

    // top.box.cbox
    Widget cbox = MW ("cbox", boxWidgetClass, box,
		      XtNorientation, XtorientVertical,
		      0);
    // top.box.cbox.preview
    Widget pbox = MW ("pbox", boxWidgetClass, cbox,
		      XtNorientation, XtorientVertical,
		      // XtNbackground, XtDefaultForeground,
		      0);
    (void)MW ("prev_label", labelWidgetClass, pbox, XtNlabel, "Preview", NULL);
    itsPreview = itsView.create_preview_widget (pbox);

    // top.???.speed

    Widget sbox = MW ("sbox", boxWidgetClass, cbox,
		      XtNorientation, XtorientVertical,
		      // XtNborder , XtDefaultBackground, // invisible
		      // XtNbackground, XtDefaultForeground,
		      0);
    // FIXME-bw/21-Jan-99: A scroller may not usable to choose a
    // discrete difficulty (needed at least by hiscore lists)
    // NOTE-bw/21-Jan-99: A hiscore list is not important (but boring)!
    /*Widget speed_label =*/ MW ("speed_label", labelWidgetClass, sbox, XtNlabel, "Speed", NULL);
    Widget speedWid = MW ("speed", scrollbarWidgetClass, sbox,
			   XtNwidth, 10,
			   XtNheight, 80,
			   //XtNfromHoriz, speed_label,
			   (void *) 0);
    XtAddCallback (speedWid, XtNjumpProc, CB_speed_jump, (XtPointer)this);
    //  XtAddCallback (speedWid, XtNscrollProc, CB_speed_scroll, (XtPointer)this);
    //(void)MW ("speed_label2", labelWidgetClass, bbox, XtNlabel, "Fast", NULL);


    // top.box.bbox
    Widget bbox = MW ("bbox", boxWidgetClass, cbox,
		      XtNorientation, XtorientVertical, (void *)0);
    // top.box.bbox.Quit
    XtAddCallback(MW ("Quit", commandWidgetClass, bbox, (void *)0),
		  XtNcallback, Stop, (void *)0);
    // top.box.bbox.Start
    XtAddCallback (MW ("Start", commandWidgetClass, bbox, (void *)0),
		   XtNcallback, CB_start, (XtPointer) this);
    // top.box.bbox.Pause
    XtAddCallback (MW ("Pause", toggleWidgetClass, bbox, (void *)0),
		   XtNcallback, CB_toggle_pause, (XtPointer) this);
    // top.box.bbox.Look
    XtAddCallback (MW ("Look", commandWidgetClass, bbox, (void *)0),
		   XtNcallback, CB_toggle_look, (XtPointer) this);

    // top.viewport
    itsPlayWid = itsView.create_play_widget (box);

#undef MW
  }

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
    wa.event_mask = (ExposureMask | KeyPressMask | KeyReleaseMask | StructureNotifyMask);
    XChangeWindowAttributes (dpy(), XtWindow (itsTopWidget), CWEventMask, &wa);
    wa.event_mask = (ExposureMask | StructureNotifyMask);
    XChangeWindowAttributes (dpy(), XtWindow (itsPlayWid), CWEventMask, &wa);
    XChangeWindowAttributes (dpy(), XtWindow (itsPreview), CWEventMask, &wa);
  }

  if (!itsView.init_after_realize_widgets ()) throw;

}

