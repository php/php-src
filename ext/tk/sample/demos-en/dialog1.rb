#
# a dialog box with a local grab (called by 'widget')
#
class TkDialog_Demo1 < TkDialog
  ###############
  private
  ###############
  def title
    "Dialog with local grab"
  end

  def message
    'This is a modal dialog box.  It uses Tk\'s "grab" command to create a "local grab" on the dialog box.  The grab prevents any pointer-related events from getting to any other windows in the application until you have answered the dialog by invoking one of the buttons below.  However, you can still interact with other applications.'
  end

  def bitmap
    'info'
  end

  def default_button
    0
  end

  def buttons
    # ["Dismiss", "", "Show Code"]
    ["OK", "Cancel", "Show Code"]
  end
end

ret =  TkDialog_Demo1.new('message_config'=>{'wraplength'=>'4i'}).value
case ret
when 0
  print "You pressed OK\n"
when 1
  print "You pressed Cancel\n"
when 2
  showCode 'dialog1'
end
