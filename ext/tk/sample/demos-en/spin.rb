# spin.rb --
#
# This demonstration script creates several spinbox widgets.
#
# based on Tcl/Tk8.4.4 widget demos

if defined?($spin_demo) && $spin_demo
  $spin_demo.destroy
  $spin_demo = nil
end

$spin_demo = TkToplevel.new {|w|
  title("Spinbox Demonstration")
  iconname("spin")
  positionWindow(w)
}

base_frame = TkFrame.new($spin_demo).pack(:fill=>:both, :expand=>true)

TkLabel.new(base_frame,
            :font=>$font, :wraplength=>'5i', :justify=>:left,
            :text=><<EOL).pack(:side=>:top)
Three different spin-boxes are displayed below.  \
You can add characters by pointing, clicking and typing.  \
The normal Motif editing characters are supported, along with \
many Emacs bindings.  For example, Backspace and Control-h \
delete the character to the left of the insertion cursor and \
Delete and Control-d delete the chararacter to the right of the \
insertion cursor.  For values that are too large to fit in the \
window all at once, you can scan through the value by dragging \
with mouse button2 pressed.  Note that the first spin-box will \
only permit you to type in integers, and the third selects from \
a list of Australian cities.
If your Tk library linked to Ruby doesn't include a 'spinbox' widget, \
this demo doesn't work. Please use later version of Tk \
which supports a 'spinbox' widget.
EOL

TkFrame.new(base_frame){|f|
  pack(:side=>:bottom, :fill=>:x, :pady=>'2m')

  TkButton.new(f, :text=>'Dismiss', :width=>15, :command=>proc{
                 $spin_demo.destroy
                 $spin_demo = nil
               }).pack(:side=>:left, :expand=>true)

  TkButton.new(f, :text=>'See Code', :width=>15, :command=>proc{
                 showCode 'spin'
               }).pack(:side=>:left, :expand=>true)
}

australianCities = [
    'Canberra', 'Sydney', 'Melbourne', 'Perth', 'Adelaide',
    'Brisbane', 'Hobart', 'Darwin', 'Alice Springs'
]

[
  TkSpinbox.new(base_frame, :from=>1, :to=>10, :width=>10, :validate=>:key,
                :validatecommand=>[
                  proc{|s| s == '' || /^[+-]?\d+$/ =~ s }, '%P'
                ]),
  TkSpinbox.new(base_frame, :from=>0, :to=>3, :increment=>0.5,
                :format=>'%05.2f', :width=>10),
  TkSpinbox.new(base_frame, :values=>australianCities, :width=>10)
].each{|sbox| sbox.pack(:side=>:top, :pady=>5, :padx=>10)}
