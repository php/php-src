#
# a dialog box with a global grab (called by 'widget')
#
class TkDialog_Demo2 < TkDialog
  ###############
  private
  ###############
  def title
    "Dialog with global grab"
  end

  def message
    "This dialog box uses a global grab, so it prevents you from interacting with anything on your display until you invoke one of the buttons below.  Global grabs are almost always a bad idea; don't use them unless you're truly desperate."
  end

  def bitmap
    'info'
  end

  def default_button
    0
  end

  def buttons
    ["OK", "Cancel", "Show Code"]
  end
end

ret =  TkDialog_Demo2.new('message_config'=>{'wraplength'=>'4i'},
                          'prev_command'=>proc{|dialog|
                            Tk.after 100, proc{dialog.grab('global')}
                          }).value
case ret
when 0
  print "\You pressed OK\n"
when 1
  print "You pressed Cancel\n"
when 2
  showCode 'dialog2'
end

