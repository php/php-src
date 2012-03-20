#
# tkballoonhelp.rb : simple balloon help widget
#                       by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
# Add a balloon help to a widget.
# This widget has only poor featureas. If you need more useful features,
# please try to use the Tix extension of Tcl/Tk under Ruby/Tk.
#
# The interval time to display a balloon help is defined 'interval' option
# (default is 750ms).
#
require 'tk'

module Tk
  module RbWidget
    class BalloonHelp<TkLabel
    end
  end
end
class Tk::RbWidget::BalloonHelp<TkLabel
  DEFAULT_FOREGROUND = 'black'
  DEFAULT_BACKGROUND = 'white'
  DEFAULT_INTERVAL   = 750

  def _balloon_binding(interval)
    @timer = TkAfter.new(interval, 1, proc{show})
    def @timer.interval(val)
      @sleep_time = val
    end
    @bindtag = TkBindTag.new
    @bindtag.bind('Enter',  proc{@timer.start})
    @bindtag.bind('Motion', proc{@timer.restart; erase})
    @bindtag.bind('Any-ButtonPress', proc{@timer.restart; erase})
    @bindtag.bind('Leave',  proc{@timer.stop; erase})
    tags = @parent.bindtags
    idx = tags.index(@parent)
    unless idx
      ppath = TkComm.window(@parent.path)
      idx = tags.index(ppath) || 0
    end
    tags[idx,0] = @bindtag
    @parent.bindtags(tags)
  end
  private :_balloon_binding

  def initialize(parent=nil, keys={})
    @parent = parent || Tk.root

    @frame = TkToplevel.new(@parent)
    @frame.withdraw
    @frame.overrideredirect(true)
    @frame.transient(TkWinfo.toplevel(@parent))
    @epath = @frame.path

    if keys
      keys = _symbolkey2str(keys)
    else
      keys = {}
    end

    @command = keys.delete('command')

    @interval = keys.delete('interval'){DEFAULT_INTERVAL}
    _balloon_binding(@interval)

    # @label = TkLabel.new(@frame, 'background'=>'bisque').pack
    @label = TkLabel.new(@frame,
                         'foreground'=>DEFAULT_FOREGROUND,
                         'background'=>DEFAULT_BACKGROUND).pack
    @label.configure(_symbolkey2str(keys)) unless keys.empty?
    @path = @label
  end

  def epath
    @epath
  end

  def interval(val)
    if val
      @timer.interval(val)
    else
      @interval
    end
  end

  def command(cmd = Proc.new)
    @command = cmd
    self
  end

  def show
    x = TkWinfo.pointerx(@parent)
    y = TkWinfo.pointery(@parent)
    @frame.geometry("+#{x+1}+#{y+1}")

    if @command
      case @command.arity
      when 0
        @command.call
      when 2
        @command.call(x - TkWinfo.rootx(@parent), y - TkWinfo.rooty(@parent))
      when 3
        @command.call(x - TkWinfo.rootx(@parent), y - TkWinfo.rooty(@parent),
                      self)
      else
        @command.call(x - TkWinfo.rootx(@parent), y - TkWinfo.rooty(@parent),
                      self, @parent)
      end
    end

    @frame.deiconify
    @frame.raise

    begin
      @org_cursor = @parent.cget('cursor')
    rescue
      @org_cursor = @parent['cursor']
    end
    begin
      @parent.configure('cursor', 'crosshair')
    rescue
      @parent.cursor('crosshair')
    end
  end

  def erase
    begin
      @parent.configure('cursor', @org_cursor)
    rescue
      @parent.cursor(@org_cursor)
    end
    @frame.withdraw
  end

  def destroy
    @frame.destroy
  end
end

################################################
# test
################################################
if __FILE__ == $0
  TkButton.new('text'=>'This button has a balloon help') {|b|
    pack('fill'=>'x')
    Tk::RbWidget::BalloonHelp.new(b, 'text'=>' Message ')
  }
  TkButton.new('text'=>'This button has another balloon help') {|b|
    pack('fill'=>'x')
    Tk::RbWidget::BalloonHelp.new(b,
                        'text'=>"CONFIGURED MESSAGE\nchange colors, and so on",
                        'interval'=>200, 'font'=>'courier',
                        'background'=>'gray', 'foreground'=>'red')
  }

  sb = TkScrollbox.new.pack(:fill=>:x)
  sb.insert(:end, *%w(aaa bbb ccc ddd eee fff ggg hhh iii jjj kkk lll mmm))
=begin
  # CASE1 : command takes no arguemnt
  bh = Tk::RbWidget::BalloonHelp.new(sb, :interval=>500,
                           :relief=>:ridge, :background=>'white',
                           :command=>proc{
                             y = TkWinfo.pointery(sb) - TkWinfo.rooty(sb)
                             bh.text "current index == #{sb.nearest(y)}"
                           })
=end
=begin
  # CASE2 : command takes 2 arguemnts
  bh = Tk::RbWidget::BalloonHelp.new(sb, :interval=>500,
                           :relief=>:ridge, :background=>'white',
                           :command=>proc{|x, y|
                             bh.text "current index == #{sb.nearest(y)}"
                           })
=end
=begin
  # CASE3 : command takes 3 arguemnts
  Tk::RbWidget::BalloonHelp.new(sb, :interval=>500,
                      :relief=>:ridge, :background=>'white',
                      :command=>proc{|x, y, bhelp|
                        bhelp.text "current index == #{sb.nearest(y)}"
                      })
=end
=begin
  # CASE4a : command is a Proc object and takes 4 arguemnts
  cmd = proc{|x, y, bhelp, parent|
    bhelp.text "current index == #{parent.nearest(y)}"
  }

  Tk::RbWidget::BalloonHelp.new(sb, :interval=>500,
                      :relief=>:ridge, :background=>'white',
                      :command=>cmd)

  sb2 = TkScrollbox.new.pack(:fill=>:x)
  sb2.insert(:end, *%w(AAA BBB CCC DDD EEE FFF GGG HHH III JJJ KKK LLL MMM))
  Tk::RbWidget::BalloonHelp.new(sb2, :interval=>500,
                      :padx=>5, :relief=>:raised,
                      :background=>'gray25', :foreground=>'white',
                      :command=>cmd)
=end
#=begin
  # CASE4b : command is a Method object and takes 4 arguemnts
  def set_msg(x, y, bhelp, parent)
    bhelp.text "current index == #{parent.nearest(y)}"
  end
  cmd = self.method(:set_msg)

  Tk::RbWidget::BalloonHelp.new(sb, :interval=>500,
                                :relief=>:ridge, :background=>'white',
                                :command=>cmd)

  sb2 = TkScrollbox.new.pack(:fill=>:x)
  sb2.insert(:end, *%w(AAA BBB CCC DDD EEE FFF GGG HHH III JJJ KKK LLL MMM))
  Tk::RbWidget::BalloonHelp.new(sb2, :interval=>500,
                                :padx=>5, :relief=>:raised,
                                :background=>'gray25', :foreground=>'white',
                                :command=>cmd)
#=end

  Tk.mainloop
end
