#!/usr/bin/env ruby
#
# This script implements the "hv" application.  Type "hv FILE" to
# view FILE as HTML.
#
# This application is used for testing the HTML widget.  It can
# also server as an example of how to use the HTML widget.
#
require 'tk'
require 'tkextlib/tkHTML'

root = TkRoot.new(:title=>'HTML File Viewer', :iconname=>'HV')

file = ARGV[0]

#
# These images are used in place of GIFs or of form elements
#
biggray = TkPhotoImage.new(:data=><<'EOD')
    R0lGODdhPAA+APAAALi4uAAAACwAAAAAPAA+AAACQISPqcvtD6OctNqLs968+w+G4kiW5omm
    6sq27gvH8kzX9o3n+s73/g8MCofEovGITCqXzKbzCY1Kp9Sq9YrNFgsAO///
EOD

smgray = TkPhotoImage.new(:data=><<'EOD')
    R0lGODdhOAAYAPAAALi4uAAAACwAAAAAOAAYAAACI4SPqcvtD6OctNqLs968+w+G4kiW5omm
    6sq27gvH8kzX9m0VADv/
EOD

nogifbig = TkPhotoImage.new(:data=><<'EOD')
    R0lGODdhJAAkAPEAAACQkADQ0PgAAAAAACwAAAAAJAAkAAACmISPqcsQD6OcdJqKM71PeK15
    AsSJH0iZY1CqqKSurfsGsex08XuTuU7L9HywHWZILAaVJssvgoREk5PolFo1XrHZ29IZ8oo0
    HKEYVDYbyc/jFhz2otvdcyZdF68qeKh2DZd3AtS0QWcDSDgWKJXY+MXS9qY4+JA2+Vho+YPp
    FzSjiTIEWslDQ1rDhPOY2sXVOgeb2kBbu1AAADv/
EOD

nogifsm = TkPhotoImage.new(:data=><<'EOD')
    R0lGODdhEAAQAPEAAACQkADQ0PgAAAAAACwAAAAAEAAQAAACNISPacHtD4IQz80QJ60as25d
    3idKZdR0IIOm2ta0Lhw/Lz2S1JqvK8ozbTKlEIVYceWSjwIAO///
EOD

#
# define variables
#
ul_hyper = TkVariable.new(0)
show_tbl = TkVariable.new(0)
show_img = TkVariable.new(1)

#
# A font chooser routine.
#
# html[:fontcommand] = pick_font
pick_font = proc{|size, attrs|
  puts "FontCmd: #{size} #{attrs}"
  [ ((attrs =~ /fixed/)? 'courier': 'charter'),
    (12 * (1.2**(size.to_f - 4.0))).to_i,
    ((attrs =~ /italic/)? 'italic': 'roman'),
    ((attrs =~ /bold/)? 'bold': 'normal') ].join(' ')
}

#
# This routine is called for each form element
#
form_cmd = proc{|n, cmd, style, *args|
  # puts "FormCmd: $n $cmd $args"
  case cmd
  when 'select', 'textarea', 'input'
    TkLabel.new(:widgetname=>args[0], :image=>nogifsm)
  end
}

#
# This routine is called for every <IMG> markup
#
images   = {}
old_imgs = {}
big_imgs = {}

hotkey   = {}

move_big_image = proc{|b|
  if big_imgs.key?(b)
    b.copy(big_imgs[b])
    big_imgs[b].delete
    big_imgs.delete(b)
    Tk.update
  end
}

image_cmd = proc{|*args|
  if show_img.bool
    smgray
  else
    fn = args[0]

    if old_imgs.key?(fn)
      images[fn] = old_imgs[fn]
      old_imgs.delete(fn)
      images[fn]

    else
      begin
        img = TkPhotoImage.new(:file=>fn)
      rescue
        smgray
      else
        if img.width * img.height > 20000
          b = TkPhotoImage.new(:width=>img.width, :height=>img.height)
          big_imgs[b] = img
          img = b
          Tk.after_idle(proc{ move_big_image.call(b) })
        end

        images[fn] = img
        img
      end
    end
  end
}

#
# This routine is called for every <SCRIPT> markup
#
script_cmd = proc{|*args|
  # puts "ScriptCmd: #{args.inspect}"
}

# This routine is called for every <APPLET> markup
#
applet_cmd = proc{|w, arglist|
  # puts "AppletCmd: w=#{w} arglist=#{arglist}"
  TkLabel.new(w, :text=>"The Applet #{w}", :bd=>2, :relief=>raised)
}

#
# Construct the main HTML viewer
#
html = Tk::HTML_Widget.new(:padx=>5, :pady=>9,
                           :formcommand=>form_cmd,
                           :imagecommand=>image_cmd,
                           :scriptcommand=>script_cmd,
                           :appletcommand=>applet_cmd,
                           :underlinehyperlinks=>0,
                           :bg=>'white', :tablerelief=>:raised)
vscr = html.yscrollbar(TkScrollbar.new)
hscr = html.xscrollbar(TkScrollbar.new)

Tk.grid(html, vscr, :sticky=>:news)
Tk.grid(hscr,       :sticky=>:ew)
Tk.root.grid_columnconfigure(0, :weight=>1)
Tk.root.grid_columnconfigure(1, :weight=>0)
Tk.root.grid_rowconfigure(0, :weight=>1)
Tk.root.grid_rowconfigure(1, :weight=>0)

#
# This procedure is called when the user clicks on a hyperlink.
#
priv = {}
last_file = ''

# Read a file
#
read_file = proc{|name|
  begin
    fp = open(name, 'r')
    ret = fp.read(File.size(name))
  rescue
    ret = nil
    fp = nil
    Tk.messageBox(:icon=>'error', :message=>"fail to open '#{name}'",
                  :type=>:ok)
  ensure
    fp.close if fp
  end
  ret
}

# Clear the screen.
#
clear_screen = proc{
  html.clear
  old_imgs.clear
  big_imgs.clear
  hotkey.clear
  images.each{|k, v| old_imgs[k] = v }
  images.clear
}

# Load a file into the HTML widget
#
load_file = proc{|name|
  if (doc = read_file.call(name))
    clear_screen.call
    last_file = name
    html.configure(:base=>name)
    html.parse(doc)
    old_imgs.clear
  end
}

href_binding = proc{|x, y|
  # koba & dg marking text
  html.selection_clear
  priv['mark'] = "@#{x},#{y}"
  lst = html.href(x, y)

  unless lst.size.zero?
    lnk, target = lst

    if lnk != ""
      if lnk =~ /^#{last_file}#(.*)$/
        html.yview($1)
      else
        load_file.call(lnk)
      end
    end
  end
}
html.clipping_window.bind('1', href_binding, '%x %y')

# marking text with the mouse and copying to the clipboard just with tkhtml2.0 working
html.clipping_window.bind('B1-Motion', proc{|w, x, y|
                            w.selection_set(priv['mark'], "@#{x},#{y}")
                            TkClipboard.clear
                            # avoid tkhtml0.0 errors
                            # anyone can fix this for tkhtml0.0
                            begin
                              TkClipboard.append(TkSelection.get)
                            rescue
                            end
                          }, '%W %x %y')

# This procedure is called when the user selects the File/Open
# menu option.
#
last_dir = Dir.pwd
sel_load = proc{
  filetypes = [
    ['Html Files', ['.html', '.htm']],
    ['All Files', '*']
  ]

  f = Tk.getOpenFile(:initialdir=>last_dir, :filetypes=>filetypes)
  if f != ''
    load_file.call(f)
    last_dir = File.dirname(f)
  end
}

# Refresh the current file.
#
refresh = proc{|*args|
  load_file.call(last_file)
}

# This binding changes the cursor when the mouse move over
# top of a hyperlink.
#
Tk::HTML_Widget::ClippingWindow.bind('Motion', proc{|w, x, y|
                                       parent = w.winfo_parent
                                       url = parent.href(x, y)
                                       unless url.empty?
                                         parent[:cursor] = 'hand2'
                                       else
                                         parent[:cursor] = ''
                                       end
                                     }, '%W %x %y')
#
# Setup menu
#
menu_spec = [
  [['File', 0],
    ['Open',    sel_load, 0],
    ['Refresh', refresh,   0],
    '---',
    ['Exit', proc{exit}, 1]],

  [['View', 0],
    ['Underline Hyperlinks', ul_hyper],
    ['Show Table Structure', show_tbl],
    ['Show Images',          show_img]]
]

mbar = Tk.root.add_menubar(menu_spec)

#
# Setup trace
#
ul_hyper.trace('w', proc{
                 html[:underlinehyperlinks] = ul_hyper.value
                 refresh.call
               })

show_tbl.trace('w', proc{
                 if show_tbl.bool
                   html[:tablerelief] = :flat
                 else
                   html[:tablerelief] = :raised
                 end
                 refresh.call
               })

show_img.trace('w', refresh)

# If an arguent was specified, read it into the HTML widget.
#
Tk.update
if file && file != ""
  load_file.call(file)
end

#####################################

Tk.mainloop
