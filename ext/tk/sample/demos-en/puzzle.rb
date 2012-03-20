# puzzle.rb
#
# This demonstration script creates a 15-puzzle game using a collection
# of buttons.
#
# widet demo 'puzzle' (called by 'widget')
#

# toplevel widget
if defined?($puzzle_demo) && $puzzle_demo
  $puzzle_demo.destroy
  $puzzle_demo = nil
end

# demo toplevel widget
$puzzle_demo = TkToplevel.new {|w|
  title("15-Puzzle Demonstration")
  iconname("15-Puzzle")
  positionWindow(w)
}

base_frame = TkFrame.new($puzzle_demo).pack(:fill=>:both, :expand=>true)

# label
msg = TkLabel.new(base_frame) {
  font $font
  wraplength '4i'
  justify 'left'
  text "A 15-puzzle appears below as a collection of buttons.  Click on any of the pieces next to the space, and that piece will slide over the space.  Continue this until the pieces are arranged in numerical order from upper-left to lower-right."
}
msg.pack('side'=>'top')

# frame
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    text 'Dismiss'
    command proc{
      tmppath = $puzzle_demo
      $puzzle_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'Show Code'
    command proc{showCode 'puzzle'}
  }.pack('side'=>'left', 'expand'=>'yes')

}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# frame

# Special trick: select a darker color for the space by creating a
# scrollbar widget and using its trough color.
begin
  if Tk.windowingsystem() == 'aqua'
    frameWidth  = 168
    frameHeight = 168
  elsif Tk.default_widget_set == :Ttk
    frameWidth  = 148
    frameHeight = 124
  else
    frameWidth  = 120
    frameHeight = 120
  end
rescue
  frameWidth  = 120
  frameHeight = 120
end

# depend_on_button_width = true
depend_on_button_width = false

s = TkScrollbar.new(base_frame)
base = TkFrame.new(base_frame) {
  width  frameWidth
  height frameHeight
  borderwidth 2
  relief 'sunken'
  bg s['troughcolor']
}
s.destroy
base.pack('side'=>'top', 'padx'=>'1c', 'pady'=>'1c')

def def_puzzleswitch_proc(w, num)
  proc{puzzleSwitch w, num}
end

$xpos = {}
$ypos = {}
order = [3,1,6,2,5,7,15,13,4,11,8,9,14,10,12]
(0..14).each{|i|
  num = order[i]
  $xpos[num] = (i % 4) * 0.25
  $ypos[num] = (i / 4) * 0.25
  TkButton.new(base) {|w|
    relief 'raised'
    text num
    highlightthickness 0
    command def_puzzleswitch_proc(w, num)
    if depend_on_button_width && (w.winfo_reqwidth * 4 > base.width)
      base.width = w.winfo_reqwidth * 4
    end
  }.place('relx'=>$xpos[num], 'rely'=>$ypos[num],
          'relwidth'=>0.25, 'relheight'=>0.25)
}
$xpos['space'] = 0.75
$ypos['space'] = 0.75


# puzzleSwitch --
# This procedure is invoked when the user clicks on a particular button;
# if the button is next to the empty space, it moves the button into the
# empty space.

def puzzleSwitch(w, num)
  if ( ($ypos[num] >= ($ypos['space'] - 0.01))     \
      && ($ypos[num] <= ($ypos['space'] + 0.01))   \
      && ($xpos[num] >= ($xpos['space'] - 0.26))   \
      && ($xpos[num] <= ($xpos['space'] + 0.26)))  \
    || (($xpos[num] >= ($xpos['space'] - 0.01))    \
        && ($xpos[num] <= ($xpos['space'] + 0.01)) \
        && ($ypos[num] >= ($ypos['space'] - 0.26)) \
        && ($ypos[num] <= ($ypos['space'] + 0.26)))
    tmp = $xpos['space']
    $xpos['space'] = $xpos[num]
    $xpos[num] = tmp
    tmp = $ypos['space']
    $ypos['space'] = $ypos[num]
    $ypos[num] = tmp
    w.place('relx'=>$xpos[num], 'rely'=>$ypos[num])
  end
end

