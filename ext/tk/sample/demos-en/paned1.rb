# paned1.rb
#
# This demonstration script creates a toplevel window containing
# a paned window that separates two windows horizontally.
#
# based on "Id: paned1.tcl,v 1.1 2002/02/22 14:07:01 dkf Exp"

if defined?($paned1_demo) && $paned1_demo
  $paned1_demo.destroy
  $paned1_demo = nil
end

$paned1_demo = TkToplevel.new {|w|
  title("Horizontal Paned Window Demonstration")
  iconname("paned1")
  positionWindow(w)
}

base_frame = TkFrame.new($paned1_demo).pack(:fill=>:both, :expand=>true)

TkLabel.new(base_frame,
            :font=>$font, :wraplength=>'4i', :justify=>:left,
            :text=><<EOL).pack(:side=>:top)
The sash between the two coloured windows below can be used to divide the area between them.  Use the left mouse button to resize without redrawing by just moving the sash, and use the middle mouse button to resize opaquely (always redrawing the windows in each position.)
If your Tk library linked to Ruby doesn't include a 'panedwindow', this demo doesn't work. Please use later version of Tk which supports a 'panedwindow'.
EOL

# The bottom buttons
TkFrame.new(base_frame){|f|
  pack(:side=>:bottom, :fill=>:x, :pady=>'2m')

  TkButton.new(f, :text=>'Dismiss', :width=>15, :command=>proc{
                 $paned1_demo.destroy
                 $paned1_demo = nil
               }).pack(:side=>:left, :expand=>true)

  TkButton.new(f, :text=>'See Code', :width=>15, :command=>proc{
                 showCode 'paned1'
               }).pack(:side=>:left, :expand=>true)
}

TkPanedwindow.new(base_frame, :orient=>:horizontal){|f|
  add(Tk::Label.new(f, :text=>"This is the\nleft side", :bg=>'yellow'),
      Tk::Label.new(f, :text=>"This is the\nright side", :bg=>'cyan'))

  pack(:side=>:top, :expand=>true, :fill=>:both, :pady=>2, :padx=>'2m')
}
