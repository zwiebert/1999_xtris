#include "gui_x.hh"

#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xutil.h>


#define MAX_CVALUE 65535	/* per X11 definition */

bool
X_Play_Field_View::make_palette ()
{
  for (int i=0; i < 8; ++i)
    {
      XColor xcol;

      xcol.red = MAX_CVALUE * ((i&2)==0);
      xcol.green = MAX_CVALUE * ((i&4)==0);
      xcol.blue = MAX_CVALUE * ((i&1)==0);
      XAllocColor (dpy, DefaultColormap (dpy, scr_nmb), &xcol);
      palette[i] = XCreateGC (dpy, RootWindow (dpy, scr_nmb), 0, 0);
      XSetForeground (dpy, palette[i], xcol.pixel);
    }
  return true;
}

#if 0
static void
draw_testpic (Display *dpy, Window win,
	      int x, int y,
	      unsigned width, unsigned height,
	      GC *gc_arr,
	      unsigned nmb_colors)
{
  for (unsigned i=0; i < nmb_colors; ++i)
    {
      XFillRectangle (dpy, win, gc_arr[i], x, y,
		      width / nmb_colors, height);
      x += width / nmb_colors;
    }
}
#endif

/* Draw a closing curtain to cover playfield after games is ending */
void
X_Play_Field_View::draw_curtain (bool defer)
{
  const int h = height / itsRows;

  for (unsigned row=0; row < itsRows; ++row)
    {
      for (unsigned col=0; col < itsCols; ++col)
	{
	  XFillRectangle (dpy, win, palette[magenta],
			  0, row * h,
			  width, h);
	  XFillRectangle (dpy, win, palette[black],
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
X_Play_Field_View::draw_flush ()
{
  XSync (dpy, False);
  //XFlush (dpy);
}

void
X_Play_Field_View::draw_clear (bool defer)
{
  XFillRectangle (dpy, win, palette[white], 0, 0, width, height);
  if (!defer)
    draw_flush ();
}

bool
X_Play_Field_View::open_window ()
{
  if (!(dpy = XOpenDisplay (0)))
   ;
  else
    {
      XSetWindowAttributes win_attr;

      scr_nmb = DefaultScreen (dpy);
      scr_ptr = ScreenOfDisplay (dpy, scr_nmb);
      win_attr.event_mask = (ExposureMask | ButtonPressMask
			     | KeyPressMask);
      win_attr.override_redirect = True;
      win = XCreateWindow (dpy, RootWindow (dpy, scr_nmb),
			   0, 0, width, height, 0,
			   CopyFromParent,
			   InputOutput,
			   CopyFromParent,
			   (CWOverrideRedirect | CWEventMask), &win_attr);

      if (XWMHints *hints = XAllocWMHints())
	{
	  hints->flags = InputHint;
	  hints->input = True;
	  XSetWMHints (dpy, win, hints);
	  XFree (hints);
	}


      if (XClassHint *hint = XAllocClassHint())
	{
	  hint->res_name = "Tetris";
	  hint->res_class = "Tetris";
	  XSetClassHint (dpy, win, hint);
	  XFree (hint);
	}


      XMapWindow (dpy, win);
      XRaiseWindow (dpy, win);
      return true;
    }
  return false;
}

#if 0
      while (1)
	{
	  XEvent e;

	  XNextEvent (dpy, &e);
	  switch(e.type)
	    {
	      int i, j;

	    case KeyPress:
	      switch (e.xkey.keycode)
		{
		case 10:
		  XFillRectangle (dpy, win, palette[black], 0, 0,
				  width, height);
		  break;

		case 11:
		  XFillRectangle (dpy, win, palette[white], 0, 0,
				  width, height);
		  break;

		case 12:
		  XFillRectangle (dpy, win, palette[red], 0, 0,
				  width, height);
		  break;

		case 13:
		  XFillRectangle (dpy, win, palette[green], 0, 0,
				  width, height);
		  break;

		case 14:
		  XFillRectangle (dpy, win, palette[blue], 0, 0,
				  width, height);
		  break;

		case 15:
		  draw_testpic (dpy, win, 0, 0, width, height,
				palette, NMB_colors);
		  break;

		case 16:
		  XFillRectangle (dpy, win, palette[black], 0, 0,
				  width, height);
		  draw_grid (dpy, win, palette[white],
			     0, 0, width, height,
			     13, 10);
		  break;
		  
		case 17:
		  XFillRectangle (dpy, win, palette[black], 0, 0,
				  width, height);
		  draw_points (dpy, win, palette[white],
			       0, 0, width, height,
			       13, 10);
		  break;
		  
		case 18:
		  XFillRectangle (dpy, win, palette[black], 0, 0,
				  width, height);
		  draw_chessboard (dpy, win, palette[white],
				   0, 0, width, height,
				   13, 10);
		  break;
		  
		default:
		  fprintf (stderr, "button: %d\n", e.xkey.keycode);
		}
	      break;

	    case Expose:

	      if (e.xexpose.count)
		break;

	      draw_testpic (dpy, win, 0, 0, width, height,
			    palette, 8);
	      XFlush (dpy);
  
	      break;
	  
	    default:
	      /* the first button press will exit the program */
	      return EXIT_SUCCESS;

	    }
	} 
#endif


X_Play_Field_View::~X_Play_Field_View ()
{
  XDestroyWindow (dpy, win);
}

X_Play_Field_View::X_Play_Field_View (Play_Field *model)
  : Play_Field_View (model),
    dpy (0), scr_ptr (0), win (0), scr_nmb (0),
    itsRows (model->get_height ()),
    itsCols (model->get_width ()),
    width (300), height (400),
    itsDamageMap (new Stone_Atom [itsRows * itsCols])
{
  bzero (palette, sizeof palette);
  for (unsigned i=0; i < itsRows * itsCols; ++i)
    itsDamageMap[i] = SA_BORDER; // force refresh

  if (!open_window ())
    ;
  else if (!make_palette ())
    ;
  else while (1)
    {
      XEvent e;
	  
      XNextEvent (dpy, &e);
      switch(e.type)
	{
	case Expose:

	  if (e.xexpose.count)
	    break;

	  draw_clear ();
	  //	  XSetInputFocus(dpy, win, RevertToParent, CurrentTime);
	  return;
	}
    } 
  throw;
}

const X_Play_Field_View::my_color
X_Play_Field_View::sa2col_map[SA_Nmb] = 
{
  white,
  yellow,
  cyan,
  green,
  magenta,
  red,
  blue 
};

void
X_Play_Field_View::draw_pixel (unsigned row, unsigned column, my_color color,
			       bool defer)
{
  const int h = height / itsRows;
  const int w = width / itsCols;

  XFillRectangle (dpy, win, palette[color], column * w, row * h, w, h);
  XDrawRectangle (dpy, win, palette[black], column * w, row * h, w, h);
  if (!defer)
    draw_flush ();
}

void
X_Play_Field_View::dump_rectangle (unsigned x, unsigned y,
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
	draw_pixel (row, col, sa2col_map[sa], true);
      }
  draw_flush ();
}

void
X_Play_Field_View::update ()
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
	    draw_pixel (row, col, sa2col_map[sa], true);
	  }
      }
  draw_flush ();
}



#include <X11/keysym.h>
void
X_Play_Field_Control::process_events (bool block)
{
  while (1)
    {
      XEvent e;

      XNextEvent (itsView.dpy, &e);
      switch(e.type)
	{
	  int i, j;

	case KeyPress:
	  switch (e.xkey.keycode)
	    {
	    case 100: //XK_Left:
	    case 102: //XK_Right:
	    case 98: // XK_Up:
	    case 97: // XK_Down:
	    default:
	      fprintf (stderr, "button: %d\n", e.xkey.keycode);
	    }
	  break;

	case Expose:
	  if (e.xexpose.count)
	    break;
	  // TODO-bw/18-Jan-99
	  break;
	  
	default:
	  /* the first button press will exit the program */
	  ;//	  return;

	}
    } 
}

