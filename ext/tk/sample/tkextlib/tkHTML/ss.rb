#!/usr/bin/env ruby
#
# This script implements the "ss" application.  "ss" implements
# a presentation slide-show based on HTML slides.
#
require 'tk'
require 'tkextlib/tkHTML'

file = ARGV[0]

class TkHTML_File_Viewer
  include TkComm

# These are images to use with the actual image specified in a
# "<img>" markup can't be found.
#
@@biggray = TkPhotoImage.new(:data=><<'EOD')
    R0lGODdhPAA+APAAALi4uAAAACwAAAAAPAA+AAACQISPqcvtD6OctNqLs968+w+G4kiW5omm
    6sq27gvH8kzX9o3n+s73/g8MCofEovGITCqXzKbzCY1Kp9Sq9YrNFgsAO///
EOD

@@smgray = TkPhotoImage.new(:data=><<'EOD')
    R0lGODdhOAAYAPAAALi4uAAAACwAAAAAOAAYAAACI4SPqcvtD6OctNqLs968+w+G4kiW5omm
    6sq27gvH8kzX9m0VADv/
EOD

  def initialize(file = nil)
    @root  = TkRoot.new(:title=>'HTML File Viewer', :iconname=>'HV')
    @fswin = nil

    @html = nil
    @html_fs = nil

    @hotkey = {}

    @applet_arg = TkVarAccess.new_hash('AppletArg')

    @images   = {}
    @old_imgs = {}
    @big_imgs = {}

    @last_dir = Dir.pwd

    @last_file = ''

    @key_block = false

    Tk::HTML_Widget::ClippingWindow.bind('1',
                                         proc{|w, ksym| key_press(w, ksym)},
                                         '%W Down')
    Tk::HTML_Widget::ClippingWindow.bind('3',
                                         proc{|w, ksym| key_press(w, ksym)},
                                         '%W Up')
    Tk::HTML_Widget::ClippingWindow.bind('2',
                                         proc{|w, ksym| key_press(w, ksym)},
                                         '%W Down')

    Tk::HTML_Widget::ClippingWindow.bind('KeyPress',
                                         proc{|w, ksym| key_press(w, ksym)},
                                         '%W %K')

    ############################################
    #
    # Build the half-size view of the page
    #
    menu_spec = [
      [['File', 0],
        ['Open',        proc{sel_load()},   0],
        ['Full Screen', proc{fullscreen()}, 0],
        ['Refresh',     proc{refresh()},    0],
        '---',
        ['Exit', proc{exit}, 1]]
    ]

    mbar = @root.add_menubar(menu_spec)

    @html = Tk::HTML_Widget.new(:width=>512, :height=>384,
                                :padx=>5, :pady=>9,
                                :formcommand=>proc{|*args| form_cmd(*args)},
                                :imagecommand=>proc{|*args|
                                  image_cmd(1, *args)
                                },
                                :scriptcommand=>proc{|*args|
                                  script_cmd(*args)
                                },
                                :appletcommand=>proc{|*args|
                                  applet_cmd(*args)
                                },
                                :hyperlinkcommand=>proc{|*args|
                                  hyper_cmd(*args)
                                },
                                :fontcommand=>proc{|*args|
                                  pick_font(*args)
                                },
                                :appletcommand=>proc{|*args|
                                  run_applet('small', *args)
                                },
                                :bg=>'white', :tablerelief=>:raised)

    @html.token_handler('meta', proc{|*args| meta(@html, *args)})

    vscr = @html.yscrollbar(TkScrollbar.new)
    hscr = @html.xscrollbar(TkScrollbar.new)

    Tk.grid(@html, vscr, :sticky=>:news)
    Tk.grid(hscr,       :sticky=>:ew)
    @root.grid_columnconfigure(0, :weight=>1)
    @root.grid_columnconfigure(1, :weight=>0)
    @root.grid_rowconfigure(0, :weight=>1)
    @root.grid_rowconfigure(1, :weight=>0)

    ############################################

    @html.clipwin.focus

    # If an arguent was specified, read it into the HTML widget.
    #
    Tk.update
    if file && file != ""
      load_file(file)
    end
  end

  #
  # A font chooser routine.
  #
  # html[:fontcommand] = pick_font
  def pick_font(size, attrs)
    # puts "FontCmd: #{size} #{attrs}"
    [ ((attrs =~ /fixed/)? 'courier': 'charter'),
      (12 * (1.2**(size.to_f - 4.0))).to_i,
      ((attrs =~ /italic/)? 'italic': 'roman'),
      ((attrs =~ /bold/)? 'bold': 'normal') ].join(' ')
  end

  # This routine is called to pick fonts for the fullscreen view.
  #
  def pick_font_fs(size, attrs)
    baseFontSize = 24

    # puts "FontCmd: #{size} #{attrs}"
    [ ((attrs =~ /fixed/)? 'courier': 'charter'),
      (baseFontSize * (1.2**(size.to_f - 4.0))).to_i,
      ((attrs =~ /italic/)? 'italic': 'roman'),
      ((attrs =~ /bold/)? 'bold': 'normal')  ].join(' ')
  end

  #
  #
  def hyper_cmd(*args)
    puts "HyperlinkCommand: #{args.inspect}"
  end

  # This routine is called to run an applet
  #
  def run_applet(size, w, arglist)
    applet_arg.value = Hash[*simplelist(arglist)]

    return unless @applet_arg.key?('src')

    src = @html.remove(@applet_arg['src'])

    @applet_arg['window'] = w
    @applet_arg['fontsize'] = size

    begin
      Tk.load_tclscript(src)
    rescue => e
      puts "Applet error: #{e.message}"
    end
  end

  #
  #
  def form_cmd(n, cmd, *args)
    # p [n, cmd, *args]
  end

  #
  #
  def move_big_image(b)
    return unless @big_imgs.key?(b)
    b.copy(@big_imgs[b])
    @big_imgs[b].delete
    @big_imgs.delete(b)
  end

  def image_cmd(hs, *args)
    fn = args[0]

    if @old_imgs.key?(fn)
      return (@images[fn] = @old_imgs.delete(fn))
    end

    begin
      img = TkPhotoImage.new(:file=>fn)
    rescue
      return ((hs)? @@smallgray: @@biggray)
    end

    if hs
      img2 = TkPhotoImage.new
      img2.copy(img, :subsample=>[2,2])
      img.delete
      img = img2
    end

    if img.width * img.height > 20000
      b = TkPhotoImage.new(:width=>img.width, :height=>img.height)
      @big_imgs[b] = img
      img = b
      Tk.after_idle(proc{ move_big_image(b) })
    end

    @images[fn] = img

    img
  end

  #
  # This routine is called for every <SCRIPT> markup
  #
  def script_cmd(*args)
    # puts "ScriptCmd: #{args.inspect}"
  end

  # This routine is called for every <APPLET> markup
  #
  def applet_cmd(w, arglist)
    # puts "AppletCmd: w=#{w} arglist=#{arglist}"
    #TkLabel.new(w, :text=>"The Applet #{w}", :bd=>2, :relief=>raised)
  end

  # This binding fires when there is a click on a hyperlink
  #
  def href_binding(w, x, y)
    lst = w.href(x, y)
    unless lst.empty?
      process_url(lst)
    end
  end

  #
  #
  def sel_load
    filetypes = [
      ['Html Files', ['.html', '.htm']],
      ['All Files', '*']
    ]

    f = Tk.getOpenFile(:initialdir=>@last_dir, :filetypes=>filetypes)
    if f != ''
      load_file(f)
      @last_dir = File.dirname(f)
    end
  end

  # Clear the screen.
  #
  def clear_screen
    if @html_fs && @html_fs.exist?
      w = @html_fs
    else
      w = @html
    end
    w.clear
    @old_imgs.clear
    @big_imgs.clear
    @hotkey.clear
    @images.each{|k, v| @old_imgs[k] = v }
    @images.clear
  end

  # Read a file
  #
  def read_file(name)
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
  end

  # Process the given URL
  #
  def process_url(url)
    case url[0]
    when /^file:/
      load_file(url[0][5..-1])
    when /^exec:/
      Tk.ip_eval(url[0][5..-1].tr('\\', ' '))
    else
      load_file(url[0])
    end
  end

  # Load a file into the HTML widget
  #
  def load_file(name)
    return unless (doc = read_file(name))
    clear_screen()
    @last_file = name
    if @html_fs && @html_fs.exist?
      w = @html_fs
    else
      w = @html
    end
    w.configure(:base=>name)
    w.parse(doc)
    w.configure(:cursor=>'top_left_arrow')
    @old_imgs.clear
  end

  # Refresh the current file.
  #
  def refresh(*args)
    load_file(@last_file) if @last_file
  end

  # This routine is called whenever a "<meta>" markup is seen.
  #
  def meta(w, tag, alist)
    v = Hash[*simplelist(alist)]

    if v.key?('key') && v.key?('href')
      @hotkey[v['key']] = w.resolve(v['href'])
    end

    if v.key?('next')
      @hotkey['Down'] =v['next']
    end

    if v.key?('prev')
      @hotkey['Up'] =v['prev']
    end

    if v.key?('other')
      @hotkey['o'] =v['other']
    end
  end

  # Go from full-screen mode back to window mode.
  #
  def fullscreen_off
    @fswin.destroy
    @root.deiconify
    Tk.update
    @root.raise
    @html.clipwin.focus
    clear_screen()
    @old_imgs.clear
    refresh()
  end

  # Go from window mode to full-screen mode.
  #
  def fullscreen
    if @fswin && @fswin.exist?
      @fswin.deiconify
      Tk.update
      @fswin.raise
      return
    end

    width  =  @root.winfo_screenwidth
    height =  @root.winfo_screenheight
    @fswin = TkToplevel.new(:overrideredirect=>true,
                            :geometry=>"#{width}x#{height}+0+0")

    @html_fs = Tk::HTML_Widget.new(@fswin, :padx=>5, :pady=>9,
                                   :formcommand=>proc{|*args|
                                     form_cmd(*args)
                                   },
                                   :imagecommand=>proc{|*args|
                                     image_cmd(0, *args)
                                   },
                                   :scriptcommand=>proc{|*args|
                                     script_cmd(*args)
                                   },
                                   :appletcommand=>proc{|*args|
                                     applet_cmd(*args)
                                   },
                                   :hyperlinkcommand=>proc{|*args|
                                     hyper_cmd(*args)
                                   },
                                   :appletcommand=>proc{|*args|
                                     run_applet('big', *args)
                                   },
                                   :fontcommand=>proc{|*args|
                                     pick_font_fs(*args)
                                   },
                                   :bg=>'white', :tablerelief=>:raised,
                                   :cursor=>:tcross) {
      pack(:fill=>:both, :expand=>true)
      token_handler('meta', proc{|*args| meta(self, *args)})
    }

    clear_screen()
    @old_imgs.clear
    refresh()
    Tk.update
    @html_fs.clipwin.focus
  end

  #
  #
  def key_press(w, keysym)
    return if @key_block
    @key_block = true
    Tk.after(250, proc{@key_block = false})

    if @hotkey.key?(keysym)
      process_url(@hotkey[keysym])
    end
    case keysym
    when 'Escape'
      if @fswin && @fswin.exist?
        fullscreen_off()
      else
        fullscreen()
      end
    end
  end
end
############################################

TkHTML_File_Viewer.new(file)

Tk.mainloop
