// -*- C++ -*- MVC Paradigma
class Model;
class View;
class Control;

class Model
{
public:
  virtual ~Model ();
  virtual void attach_view (View *view);
  virtual void detach_view (View *view);
  virtual void notify_views ();
protected:
  Model ();
private:
  class Impl;
  Impl &itsImpl;
};

class View
{
public:
  virtual ~View () {}
  virtual void update ()=0;
protected:
  View () {}
};

#if 0
class Control
{
public:
  virtual ~Control () {}
  virtual void set_model (Model *model)=0;
protected:
};
#endif
