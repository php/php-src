# clrpick.rb
#
# This demonstration script prompts the user to select a color.
#
# widget demo prompts the user to select a color (called by 'widget')
#
#  Note: don't support ttk_wrapper. work with standard widgets only.
#

# toplevel widget
if defined?($clrpick_demo) && $clrpick_demo
  $clrpick_demo.destroy
  $clrpick_demo = nil
end

# demo toplevel widget
$clrpick_demo = TkToplevel.new {|w|
  title("Color Selection Dialogs")
  iconname("colors")
  positionWindow(w)
}

base_frame = TkFrame.new($clrpick_demo).pack(:fill=>:both, :expand=>true)

# label
#TkLabel.new($clrpick_demo,'font'=>$font,'wraplength'=>'4i','justify'=>'left',
Tk::Label.new($clrpick_demo,'font'=>$font,'wraplength'=>'4i','justify'=>'left',
            'text'=>"Press the buttons below to choose the foreground and background colors for the widgets in this window.").pack('side'=>'top')

# frame
#TkFrame.new($clrpick_demo) {|frame|
Tk::Frame.new($clrpick_demo) {|frame|
  # TkButton.new(frame) {
  Tk::Button.new(frame) {
    text 'Dismiss'
    command proc{
      tmppath = $clrpick_demo
      $clrpick_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  # TkButton.new(frame) {
  Tk::Button.new(frame) {
    text 'Show Code'
    command proc{showCode 'clrpick'}
  }.pack('side'=>'left', 'expand'=>'yes')
}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# button
# TkButton.new($clrpick_demo, 'text'=>'Set background color ...') {|b|
Tk::Button.new($clrpick_demo, 'text'=>'Set background color ...') {|b|
  command(proc{setColor $clrpick_demo, b, 'background',
              ['background', 'highlightbackground']})
  pack('side'=>'top', 'anchor'=>'c', 'pady'=>'2m')
}

# TkButton.new($clrpick_demo, 'text'=>'Set foreground color ...') {|b|
Tk::Button.new($clrpick_demo, 'text'=>'Set foreground color ...') {|b|
  command(proc{setColor $clrpick_demo, b, 'foreground', ['foreground']})
  pack('side'=>'top', 'anchor'=>'c', 'pady'=>'2m')
}

def setColor(w,button,name,options)
  w.grab
  initialColor = button[name]
  color = Tk.chooseColor('title'=>"Choose a #{name} color", 'parent'=>w,
                         'initialcolor'=>initialColor)
  if color != ""
    setColor_helper(w,options,color)
  end

  w.grab('release')
end

def setColor_helper(w, options, color)
  options.each{|opt|
    begin
      w[opt] = color
    rescue
    end
  }
  TkWinfo.children(w).each{|child|
    setColor_helper child, options, color
  }
end

