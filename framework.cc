#include "framework.hh"

Play_Field_View::~Play_Field_View ()
{
  if (itsModel)
    itsModel->detach_view (this);
}


void
Play_Field_View::set_model (Play_Field *model)
{
  if (itsModel) itsModel->detach_view (this);
  itsModel = model;
  if (itsModel)
    {
      itsModel->attach_view (this);
    }
}
