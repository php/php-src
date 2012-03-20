#
# tk/scrollbar.rb : treat scrollbar widget
#
require 'tk'

class Tk::Scrollbar<TkWindow
  TkCommandNames = ['scrollbar'.freeze].freeze
  WidgetClassName = 'Scrollbar'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def create_self(keys)
    @assigned = []
    @scroll_proc = proc{|*args|
      if self.orient == 'horizontal'
        @assigned.each{|w| w.xview(*args)}
      else # 'vertical'
        @assigned.each{|w| w.yview(*args)}
      end
    }

    if keys and keys != None
      unless TkConfigMethod.__IGNORE_UNKNOWN_CONFIGURE_OPTION__
        #tk_call_without_enc('scrollbar', @path, *hash_kv(keys, true))
        tk_call_without_enc(self.class::TkCommandNames[0], @path,
                            *hash_kv(keys, true))
      else
        begin
          tk_call_without_enc(self.class::TkCommandNames[0], @path,
                              *hash_kv(keys, true))
        rescue
          tk_call_without_enc(self.class::TkCommandNames[0], @path)
          keys = __check_available_configure_options(keys)
          unless keys.empty?
            begin
              tk_call_without_enc('destroy', @path)
            rescue
              # cannot destroy
              configure(keys)
            else
              # re-create widget
              tk_call_without_enc(self.class::TkCommandNames[0], @path,
                                  *hash_kv(keys, true))
            end
          end
        end
      end
    else
      #tk_call_without_enc('scrollbar', @path)
      tk_call_without_enc(self.class::TkCommandNames[0], @path)
    end
  end
  private :create_self

  def propagate_set(src_win, first, last)
    self.set(first, last)
    if self.orient == 'horizontal'
      @assigned.each{|w| w.xview('moveto', first) if w != src_win}
    else # 'vertical'
      @assigned.each{|w| w.yview('moveto', first) if w != src_win}
    end
  end

  def assign(*wins)
    begin
      self.command(@scroll_proc) if self.cget('command').cmd != @scroll_proc
    rescue Exception
      self.command(@scroll_proc)
    end
    orient = self.orient
    wins.each{|w|
      @assigned << w unless @assigned.index(w)
      if orient == 'horizontal'
        w.xscrollcommand proc{|first, last| self.propagate_set(w, first, last)}
      else # 'vertical'
        w.yscrollcommand proc{|first, last| self.propagate_set(w, first, last)}
      end
    }
    Tk.update  # avoid scrollbar trouble
    self
  end

  def assigned_list
    begin
      return @assigned.dup if self.cget('command').cmd == @scroll_proc
    rescue Exception
    end
    fail RuntimeError, "not depend on the assigned_list"
  end

  def configure(*args)
    ret = super(*args)
    # Tk.update  # avoid scrollbar trouble
    ret
  end

  #def delta(deltax=None, deltay=None)
  def delta(deltax, deltay)
    number(tk_send_without_enc('delta', deltax, deltay))
  end

  #def fraction(x=None, y=None)
  def fraction(x, y)
    number(tk_send_without_enc('fraction', x, y))
  end

  def identify(x, y)
    tk_send_without_enc('identify', x, y)
  end

  def get
    #ary1 = tk_send('get').split
    #ary2 = []
    #for i in ary1
    #  ary2.push number(i)
    #end
    #ary2
    list(tk_send_without_enc('get'))
  end

  def set(first, last)
    tk_send_without_enc('set', first, last)
    self
  end

  def activate(element=None)
    tk_send_without_enc('activate', element)
  end

  def moveto(fraction)
    tk_send_without_enc('moveto', fraction)
    self
  end

  def scroll(*args)
    tk_send_without_enc('scroll', *args)
    self
  end

  def scroll_units(num)
    scroll(num, 'units')
    self
  end

  def scroll_pages(num)
    scroll(num, 'pages')
    self
  end
end

#TkScrollbar = Tk::Scrollbar unless Object.const_defined? :TkScrollbar
#Tk.__set_toplevel_aliases__(:Tk, Tk::Scrollbar, :TkScrollbar)
Tk.__set_loaded_toplevel_aliases__('tk/scrollbar.rb', :Tk, Tk::Scrollbar,
                                   :TkScrollbar)


class Tk::XScrollbar<Tk::Scrollbar
  def create_self(keys)
    keys = {} unless keys
    keys['orient'] = 'horizontal'
    super(keys)
  end
  private :create_self
end

#TkXScrollbar = Tk::XScrollbar unless Object.const_defined? :TkXScrollbar
#Tk.__set_toplevel_aliases__(:Tk, Tk::XScrollbar, :TkXScrollbar)
Tk.__set_loaded_toplevel_aliases__('tk/scrollbar.rb', :Tk, Tk::XScrollbar,
                                   :TkXScrollbar)


class Tk::YScrollbar<Tk::Scrollbar
  def create_self(keys)
    keys = {} unless keys
    keys['orient'] = 'vertical'
    super(keys)
  end
  private :create_self
end

#TkYScrollbar = Tk::YScrollbar unless Object.const_defined? :TkYScrollbar
#Tk.__set_toplevel_aliases__(:Tk, Tk::YScrollbar, :TkYScrollbar)
Tk.__set_loaded_toplevel_aliases__('tk/scrollbar.rb', :Tk, Tk::YScrollbar,
                                   :TkYScrollbar)
