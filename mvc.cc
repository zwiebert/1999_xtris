#include "mvc.hh"
#include <list>

class Model::Impl
{
public:
  typedef std::list< View *> View_List;
  View_List views;
};

Model::~Model ()
{
  delete &itsImpl;
}

Model::Model (): itsImpl (*new Impl)
{
}

void
Model::attach_view (View *view)
{
  itsImpl.views.push_back (view);
}

void
Model::detach_view (View *view)
{
  itsImpl.views.remove (view);
}

void
Model::notify_views ()
{
  for (Impl::View_List::iterator it = itsImpl.views.begin ();
       it != itsImpl.views.end ();
       ++it)
    {
      (*it)->update (); // ???-bw/16-Jan-99
    }
}
  

