#
# tk/scrollable.rb : module for scrollable widget
#
require 'tk'

module Tk
  module XScrollable
    def xscrollcommand(cmd=Proc.new)
      configure_cmd 'xscrollcommand', cmd
      # Tk.update  # avoid scrollbar trouble
      self
    end

    def xview(*index)
      if index.size == 0
        list(tk_send_without_enc('xview'))
      else
        tk_send_without_enc('xview', *index)
        self
      end
    end
    def xview_moveto(*index)
      xview('moveto', *index)
    end
    def xview_scroll(*index)
      xview('scroll', *index)
    end

    def xscrollbar(bar=nil)
      if bar
        @xscrollbar = bar
        @xscrollbar.orient 'horizontal'
        self.xscrollcommand {|*arg| @xscrollbar.set(*arg)}
        @xscrollbar.command {|*arg| self.xview(*arg)}
        Tk.update  # avoid scrollbar trouble
      end
      @xscrollbar
    end
  end

  module YScrollable
    def yscrollcommand(cmd=Proc.new)
      configure_cmd 'yscrollcommand', cmd
      # Tk.update  # avoid scrollbar trouble
      self
    end

    def yview(*index)
      if index.size == 0
        list(tk_send_without_enc('yview'))
      else
        tk_send_without_enc('yview', *index)
        self
      end
    end
    def yview_moveto(*index)
      yview('moveto', *index)
    end
    def yview_scroll(*index)
      yview('scroll', *index)
    end

    def yscrollbar(bar=nil)
      if bar
        @yscrollbar = bar
        @yscrollbar.orient 'vertical'
        self.yscrollcommand {|*arg| @yscrollbar.set(*arg)}
        @yscrollbar.command {|*arg| self.yview(*arg)}
        Tk.update  # avoid scrollbar trouble
      end
      @yscrollbar
    end
  end

  X_Scrollable = XScrollable
  Y_Scrollable = YScrollable

  module Scrollable
    include XScrollable
    include YScrollable
  end
end
