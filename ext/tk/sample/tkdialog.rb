#!/usr/bin/env ruby
require "tk"

root = TkFrame.new
top = TkFrame.new(root) {
  relief 'raised'
  border 1
}
msg = TkMessage.new(top) {
  text "File main.c hasn't been saved to disk since \
it was last modified.  What should I do?"
  justify 'center'
  aspect 200
  font '-Adobe-helvetica-medium-r-normal--*-240*'
  pack('padx'=>5, 'pady'=>5, 'expand'=>'yes')
}
top.pack('fill'=>'both')
root.pack

bot = TkFrame.new(root) {
  relief 'raised'
  border 1
}

TkFrame.new(bot) { |left|
  relief 'sunken'
  border 1
  pack('side'=>'left', 'expand'=>'yes', 'padx'=>10, 'pady'=> 10)
  TkButton.new(left) {
    text "Save File"
    command "quit 'save'"
    pack('expand'=>'yes','padx'=>6,'pady'=> 6)
    top.bind "Enter", proc{state 'active'}
    msg.bind "Enter", proc{state 'active'}
    bot.bind "Enter", proc{state 'active'}
    top.bind "Leave", proc{state 'normal'}
    msg.bind "Leave", proc{state 'normal'}
    bot.bind "Leave", proc{state 'normal'}
    Tk.root.bind "ButtonRelease-1", proc{quit 'save'}
    Tk.root.bind "Return", proc{quit 'save'}
  }
}
TkButton.new(bot) {
  text "Quit Anyway"
  command "quit 'quit'"
  pack('side'=>'left', 'expand'=>'yes', 'padx'=>10)
}
TkButton.new(bot) {
  text "Return To Editor"
  command "quit 'return'"
  pack('side'=>'left', 'expand'=>'yes', 'padx'=>10)
}
bot.pack
root.pack('side'=>'top', 'fill'=>'both', 'expand'=>'yes')

def quit(button)
  print "You pressed the \"#{button}\" button;  bye-bye!\n"
  exit
end

Tk.mainloop
