#include "playfield.hh"

Tetris_Play_Field::Tetris_Play_Field (int width, int height)
  : Play_Field (width, height)
  , itsXPos (0), itsYPos (0), itsAngle (Stone::SAL_NORMAL)
  , itsPixMap (new Stone_Atom[(height+1) * (width+2)])
{
  bzero (itsPixMap, (height+1) * (width+2));
  for (int col=0; col < width; ++col)
    set_field (height, col, SA_BORDER);
  for (int row=0; row < height; ++row)
    set_field (row, -1, SA_BORDER);
  for (int row=0; row < height; ++row)
    set_field (row, width, SA_BORDER);
}

Tetris_Play_Field::~Tetris_Play_Field ()
{
  delete [] itsPixMap;
} 

void
Tetris_Play_Field::set_current_stone (Stone &s)
{
  itsCurrStone = &s;
  itsXPos = itsWidth / 2;
  itsYPos = 0;
  itsAngle = Stone::SAL_NORMAL;
  map_current_stone ();
}

bool
Tetris_Play_Field::test_collision () const
{
  for (unsigned col=0; col < itsCurrStone->width (); ++col)
    for (unsigned row=itsCurrStone->height (); row-- != 0;)
      if (itsCurrStone->test_pixel (row, col, itsAngle))
	if (get_field (itsYPos + row + 1, itsXPos + col))
	  return true;  // collision against bottom border or stone
	else
	  break;
  return false;
}

bool
Tetris_Play_Field::test_overlap (Stone::Stone_Angle angle) const
{
  for (unsigned col=0; col < itsCurrStone->width (); ++col)
    for (unsigned row=itsCurrStone->height (); row-- != 0;)
      if (itsCurrStone->test_pixel (row, col, angle))
	if (get_field (itsYPos + row, itsXPos + col))
	  return true;  // overlap with bottom border or stone
	else
	  break;
  return false;
}


void
Tetris_Play_Field::map_current_stone ()
{
  for (unsigned row=0; row < itsCurrStone->height (); ++row)
    for (unsigned col=0; col < itsCurrStone->width (); ++col)
      if (itsCurrStone->test_pixel (row, col, itsAngle))
	set_field (row + itsYPos, col + itsXPos, itsCurrStone->get_atom ());
}

void
Tetris_Play_Field::unmap_current_stone ()
{
  for (unsigned row=0; row < itsCurrStone->height (); ++row)
    for (unsigned col=0; col < itsCurrStone->width (); ++col)
      {
	if (itsCurrStone->test_pixel (row, col, itsAngle))
	  set_field (row + itsYPos, col + itsXPos, SA_0);
      }
}

bool
Tetris_Play_Field::rotate_stone_90_cw ()
{
  int tmp = itsAngle;

  ++tmp %= 4;

  unmap_current_stone ();  // Avoid overlap detection against ourself
  if (test_overlap ((Stone::Stone_Angle)tmp))
    {
      map_current_stone ();  // restore
      return false;
    }
  itsAngle = (Stone::Stone_Angle)tmp;
  map_current_stone ();

  return true;  // TODO-bw/18-Jan-99
}

void
Tetris_Play_Field::remove_row (unsigned n)
{
  for (unsigned row=n; row > 0; --row)
    for (unsigned col=0; col < itsWidth; ++col)
      set_field (row, col, get_field (row-1, col));
  
  for (unsigned col=0; col < itsWidth; ++col)
    set_field (0, col, SA_0);
}


bool
Tetris_Play_Field::move_stone (Direction d)
{
  if (test_collision ())
    return false;

  switch (d)
    {
    case DIR_SOUTH:
      unmap_current_stone ();
      ++itsYPos;
      map_current_stone ();
      return true;
    case DIR_WEST:
      unmap_current_stone ();
      --itsXPos;
      if (test_overlap (itsAngle))
	{
	  ++itsXPos;
	  map_current_stone ();
	  return false;
	}
      else
	{
	  map_current_stone ();
	  return true;
	}
    case DIR_EAST:
      unmap_current_stone ();
      ++itsXPos;
      if (test_overlap (itsAngle))
	{
	  --itsXPos;
	  map_current_stone ();
	  return false;
	}
      else
	{
	  map_current_stone ();
	  return true;
	}
    }
  return false;
}

