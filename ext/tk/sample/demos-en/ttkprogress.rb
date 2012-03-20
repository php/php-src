# ttkprogress.rb --
#
# This demonstration script creates several progress bar widgets.
#
# based on "Id: ttkprogress.tcl,v 1.3 2007/12/13 15:27:07 dgp Exp"

if defined?($ttkprogress_demo) && $ttkprogress_demo
  $ttkprogress_demo.destroy
  $ttkprogress_demo = nil
end

$ttkprogress_demo = TkToplevel.new {|w|
  title("Progress Bar Demonstration")
  iconname("ttkprogress")
  positionWindow(w)
}

base_frame = TkFrame.new($ttkprogress_demo).pack(:fill=>:both, :expand=>true)

Ttk::Label.new(base_frame, :font=>$font, :wraplength=>'4i', :justify=>:left,
               :text=>Tk::UTF8_String.new(<<EOL)).pack(:side=>:top, :fill=>:x)
Below are two progress bars. \
The top one is a \\u201Cdeterminate\\u201D progress bar, \
which is used for showing how far through a defined task the program has got. \
The bottom one is an \\u201Cindeterminate\\u201D progress bar, \
which is used to show that the program is busy \
but does not know how long for. Both are run here in self-animated mode, \
which can be turned on and off using the buttons underneath.
EOL

## See Code / Dismiss buttons
Ttk::Frame.new(base_frame) {|frame|
  sep = Ttk::Separator.new(frame)
  Tk.grid(sep, :columnspan=>4, :row=>0, :sticky=>'ew', :pady=>2)
  TkGrid('x',
         Ttk::Button.new(frame, :text=>'See Code',
                         :image=>$image['view'], :compound=>:left,
                         :command=>proc{showCode 'ttkprogress'}),
         Ttk::Button.new(frame, :text=>'Dismiss',
                         :image=>$image['delete'], :compound=>:left,
                         :command=>proc{
                           $ttkprogress_demo.destroy
                           $ttkprogress_demo = nil
                         }),
         :padx=>4, :pady=>4)
  grid_columnconfigure(0, :weight=>1)
  pack(:side=>:bottom, :fill=>:x)
}

frame = Ttk::Frame.new(base_frame).pack(:fill=>:both, :expand=>true)

p1 = Ttk::Progressbar.new(frame, :mode=>:determinate)
p2 = Ttk::Progressbar.new(frame, :mode=>:indeterminate)

start = Ttk::Button.new(frame, :text=>'Start Progress',
                        :command=>proc{ p1.start; p2.start })
stop  = Ttk::Button.new(frame, :text=>'Stop Progress',
                        :command=>proc{ p1.stop; p2.stop })

Tk.grid(p1, '-', :pady=>5, :padx=>10)
Tk.grid(p2, '-', :pady=>5, :padx=>10)
Tk.grid(start, stop, :padx=>10, :pady=>5)
start.grid_configure(:sticky=>'e')
stop.grid_configure(:sticky=>'w')
frame.grid_columnconfigure(:all, :weight=>1)

