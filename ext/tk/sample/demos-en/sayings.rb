# sayings.rb
#
# This demonstration script creates a listbox that can be scrolled
# both horizontally and vertically.  It displays a collection of
# well-known sayings.
#
# listbox widget demo 'sayings' (called by 'widget')
#

# toplevel widget
if defined?($sayings_demo) && $sayings_demo
  $sayings_demo.destroy
  $sayings_demo = nil
end

# demo toplevel widget
$sayings_demo = TkToplevel.new {|w|
  title("Listbox Demonstration (well-known sayings)")
  iconname("sayings")
  positionWindow(w)
}

base_frame = TkFrame.new($sayings_demo).pack(:fill=>:both, :expand=>true)

# label
msg = TkLabel.new(base_frame) {
  font $font
  wraplength '4i'
  justify 'left'
  text "The listbox below contains a collection of well-known sayings.  You can scan the list using either of the scrollbars or by dragging in the listbox window with button 2 pressed."
}
msg.pack('side'=>'top')

# frame
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    text 'Dismiss'
    command proc{
      tmppath = $sayings_demo
      $sayings_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'Show Code'
    command proc{showCode 'sayings'}
  }.pack('side'=>'left', 'expand'=>'yes')

}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# frame
sayings_lbox = nil
TkFrame.new(base_frame, 'borderwidth'=>10) {|w|
  sv = TkScrollbar.new(w)
  sh = TkScrollbar.new(w, 'orient'=>'horizontal')
  sayings_lbox = TkListbox.new(w) {
    setgrid 1
    width  20
    height 10
    yscrollcommand proc{|first,last| sv.set first,last}
    xscrollcommand proc{|first,last| sh.set first,last}
  }
  sv.command(proc{|*args| sayings_lbox.yview(*args)})
  sh.command(proc{|*args| sayings_lbox.xview(*args)})

  if $tk_version =~ /^4\.[01]/
    sv.pack('side'=>'right', 'fill'=>'y')
    sh.pack('side'=>'bottom', 'fill'=>'x')
    sayings_lbox.pack('expand'=>'yes', 'fill'=>'y')

  else
    sayings_lbox.grid('row'=>0, 'column'=>0,
                      'rowspan'=>1, 'columnspan'=>1, 'sticky'=>'news')
    sv.grid('row'=>0, 'column'=>1,
            'rowspan'=>1, 'columnspan'=>1, 'sticky'=>'news')
    sh.grid('row'=>1, 'column'=>0,
            'rowspan'=>1, 'columnspan'=>1, 'sticky'=>'news')
    TkGrid.rowconfigure(w, 0, 'weight'=>1, 'minsize'=>0)
    TkGrid.columnconfigure(w, 0, 'weight'=>1, 'minsize'=>0)
  end

}.pack('side'=>'top', 'expand'=>'yes', 'fill'=>'y')

sayings_lbox.insert(0,
"Waste not, want not",
"Early to bed and early to rise makes a man healthy, wealthy, and wise",
"Ask not what your country can do for you, ask what you can do for your country",
"I shall return",
"NOT",
"A picture is worth a thousand words",
"User interfaces are hard to build",
"Thou shalt not steal",
"A penny for your thoughts",
"Fool me once, shame on you;  fool me twice, shame on me",
"Every cloud has a silver lining",
"Where there's smoke there's fire",
"It takes one to know one",
"Curiosity killed the cat; but satisfaction brought it back",
"Take this job and shove it",
"Up a creek without a paddle",
"I'm mad as hell and I'm not going to take it any more",
"An apple a day keeps the doctor away",
"Don't look a gift horse in the mouth"
)

