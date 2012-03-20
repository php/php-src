# msgbox.rb
#
# This demonstration script creates message boxes of various type
#
# message boxes widget demo (called by 'widget')
#

# toplevel widget
if defined?($msgbox_demo) && $msgbox_demo
  $msgbox_demo.destroy
  $msgbox_demo = nil
end

# demo toplevel widget
$msgbox_demo = TkToplevel.new {|w|
  title("Message Box Demonstration")
  iconname("messagebox")
  positionWindow(w)
}

base_frame = TkFrame.new($msgbox_demo).pack(:fill=>:both, :expand=>true)

# label
TkLabel.new(base_frame, 'font'=>$font, 'wraplength'=>'4i', 'justify'=>'left',
            'text'=>"Choose the icon and type option of the message box. Then press the \"Message Box\" button to see the message box.").pack('side'=>'top')

# frame
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    text 'Dismiss'
    command proc{
      tmppath = $msgbox_demo
      $msgbox_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'Show Code'
    command proc{showCode 'msgbox'}
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'Message Box'
    command proc{showMessageBox $msgbox_demo}
  }.pack('side'=>'left', 'expand'=>'yes')
}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# frame
$msgbox_leftframe  = TkFrame.new(base_frame)
$msgbox_rightframe = TkFrame.new(base_frame)
$msgbox_leftframe .pack('side'=>'left', 'expand'=>'yes', 'fill'=>'y',
                        'pady'=>'.5c', 'padx'=>'.5c')
$msgbox_rightframe.pack('side'=>'left', 'expand'=>'yes', 'fill'=>'y',
                        'pady'=>'.5c', 'padx'=>'.5c')

TkLabel.new($msgbox_leftframe, 'text'=>'Icon').pack('side'=>'top')
TkFrame.new($msgbox_leftframe, 'relief'=>'ridge', 'bd'=>1, 'height'=>2)\
.pack('side'=>'top', 'fill'=>'x', 'expand'=>'no')

$msgboxIcon = TkVariable.new('info')
['error', 'info', 'question', 'warning'].each {|icon|
  TkRadioButton.new($msgbox_leftframe, 'text'=>icon, 'variable'=>$msgboxIcon,
                    'relief'=>'flat', 'value'=>icon, 'width'=>16,
                    'anchor'=>'w').pack('side'=>'top', 'pady'=>2,
                                        'anchor'=>'w', 'fill'=>'x')
}

TkLabel.new($msgbox_rightframe, 'text'=>'Type').pack('side'=>'top')
TkFrame.new($msgbox_rightframe, 'relief'=>'ridge', 'bd'=>1, 'height'=>2)\
.pack('side'=>'top', 'fill'=>'x', 'expand'=>'no')

$msgboxType = TkVariable.new('ok')
['abortretryignore', 'ok', 'okcancel',
  'retrycancel', 'yesno', 'yesnocancel'].each {|type|
  TkRadioButton.new($msgbox_rightframe, 'text'=>type, 'variable'=>$msgboxType,
                    'relief'=>'flat', 'value'=>type, 'width'=>16,
                    'anchor'=>'w').pack('side'=>'top', 'pady'=>2,
                                        'anchor'=>'w', 'fill'=>'x')
}

def showMessageBox(w)
  button = Tk.messageBox('icon'=>$msgboxIcon.value, 'type'=>$msgboxType.value,
                         'title'=>'Message', 'parent'=>w,
                         'message'=>"This is a \"#{$msgboxType.value}\" type messagebox with the \"#{$msgboxIcon.value}\" icon")

  Tk.messageBox('icon'=>'info', 'type'=>'ok', 'parent'=>w,
                'message'=>"You have selected  \"#{button}\"")
end

