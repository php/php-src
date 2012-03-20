# -*- coding: utf-8 -*-
#
# Based on the widget demo of Tcl/Tk8.5.2
# The following is the original copyright text.
#----------------------------------------------------------------------------
# Copyright (C) 2008 Pat Thoyts <patthoyts@users.sourceforge.net>
#
#	Calculate a Knight's tour of a chessboard.
#
#	This uses Warnsdorff's rule to calculate the next square each
#	time. This specifies that the next square should be the one that
#	has the least number of available moves.
#
#	Using this rule it is possible to get to a position where
#	there are no squares available to move into. In this implementation
#	this occurs when the starting square is d6.
#
#	To solve this fault an enhancement to the rule is that if we
#	have a choice of squares with an equal score, we should choose
#	the one nearest the edge of the board.
#
#	If the call to the Edgemost function is commented out you can see
#	this occur.
#
#	You can drag the knight to a specific square to start if you wish.
#	If you let it repeat then it will choose random start positions
#	for each new tour.
#----------------------------------------------------------------------------
require 'tk'

class Knights_Tour
  # Return a list of accessible squares from a given square
  def valid_moves(square)
    moves = []
    [
      [-1,-2], [-2,-1], [-2,1], [-1,2], [1,2], [2,1], [2,-1], [1,-2]
    ].each{|col_delta, row_delta|
      col = (square % 8) + col_delta
      row = (square.div(8)) + row_delta
      moves << (row * 8 + col) if row > -1 && row < 8 && col > -1 && col < 8
    }
    moves
  end

  # Return the number of available moves for this square
  def check_square(square)
    valid_moves(square).find_all{|pos| ! @visited.include?(pos)}.length
  end

  # Select the next square to move to. Returns -1 if there are no available
  # squares remaining that we can move to.
  def next_square(square)
    minimum = 9
    nxt = -1
    valid_moves(square).each{|pos|
      unless @visited.include?(pos)
        cnt = check_square(pos)
        if cnt < minimum
          minimum = cnt
          nxt = pos
        elsif cnt == minimum
          nxt = edgemost(nxt, pos)
        end
      end
    }
    nxt
  end

  # Select the square nearest the edge of the board
  def edgemost(nxt, pos)
    col_A = 3 - ((3.5 - nxt % 8).abs.to_i)
    col_B = 3 - ((3.5 - pos % 8).abs.to_i)
    row_A = 3 - ((3.5 - nxt.div(8)).abs.to_i)
    row_B = 3 - ((3.5 - pos.div(8)).abs.to_i)
    (col_A * row_A < col_B * row_B)? nxt : pos
  end

  # Display a square number as a standard chess square notation.
  def _N(square)
    '%c%d' % [(97 + square % 8), (square.div(8) + 1)]
  end

  # Perform a Knight's move and schedule the next move.
  def move_piece(last, square)
    @log.insert(:end, "#{@visited.length}. #{_N last} -> #{_N square}\n", '')
    @log.see(:end)
    @board.itemconfigure(1+last, :state=>:normal, :outline=>'black')
    @board.itemconfigure(1+square, :state=>:normal, :outline=>'red')
    @knight.coords(@board.coords(1+square)[0..1])
    @visited << square
    if (nxt = next_square(square)) != -1
      @after_id = Tk.after(@delay.numeric){move_piece(square, nxt) rescue nil}
    else
      @start_btn.state :normal
      if @visited.length == 64
        if @initial == square
          @log.insert :end, '周遊(closed tour)成功！'
        else
          @log.insert :end, "成功\n", {}
          Tk.after(@delay.numeric * 2){tour(rand(64))} if @continuous.bool
        end
      else
        @log.insert :end, "失敗！\n", {}
      end
    end
  end

  # Begin a new tour of the board given a random start position
  def tour(square = nil)
    @visited.clear
    @log.clear
    @start_btn.state :disabled
    1.upto(64){|n|
      @board.itemconfigure(n, :state=>:disabled, :outline=>'black')
    }
    unless square
      square = @board.find_closest(*(@knight.coords << 0 << 65))[0].to_i - 1
    end
    @initial = square
    Tk.after_idle{ move_piece(@initial, @initial) rescue nil }
  end

  def _stop
    Tk.after_cancel(@after_id) rescue nil
  end

  def _exit
    _stop
    $knightstour.destroy
  end

  def set_delay(new)
    @delay.numeric = new.to_i
  end

  def drag_start(w, x, y)
    w.dtag('selected')
    w.addtag('selected', :withtag, 'current')
    @dragging = [x, y]
  end

  def drag_motion(w, x, y)
    return unless @dragging
    w.move('selected', x - @dragging[0], y - @dragging[1])
    @dragging = [x, y]
  end

  def drag_end(w, x, y)
    square = w.find_closest(x, y, 0, 65)
    w.coords('selected', w.coords(square)[0..1])
    w.dtag('selected')
    @dragging = nil
  end

  def make_SeeDismiss
    ## See Code / Dismiss
    frame = Ttk::Frame.new($knightstour)
    sep = Ttk::Separator.new(frame)
    Tk.grid(sep, :columnspan=>4, :row=>0, :sticky=>'ew', :pady=>2)
    TkGrid('x',
           Ttk::Button.new(frame, :text=>'コード参照',
                           :image=>$image['view'], :compound=>:left,
                           :command=>proc{showCode 'knightstour'}),
           Ttk::Button.new(frame, :text=>'閉じる',
                           :image=>$image['delete'], :compound=>:left,
                           :command=>proc{
                             $knightstour.destroy
                             $knightstour = nil
                           }),
           :padx=>4, :pady=>4)
    frame.grid_columnconfigure(0, :weight=>1)
    frame
  end

  def create_gui(parent = nil)
    $knightstour.destroy rescue nil
    $knightstour = Tk::Toplevel.new(parent, :title=>"Knight's tour")
    $knightstour.withdraw
    base_f = Ttk::Frame.new($knightstour)
    @board = Tk::Canvas.new(base_f, :width=>240, :height=>240)
    @log = Tk::Text.new(base_f, :width=>12, :height=>1,
                        :font=>'Arial 8', :background=>'white')
    scr = @log.yscrollbar(Ttk::Scrollbar.new(base_f))

    @visited = []
    @delay = TkVariable.new(600)
    @continuous = TkVariable.new(false)

    tool_f = Ttk::Frame.new($knightstour)
    label = Ttk::Label.new(tool_f, :text=>'実行速度')
    scale = Ttk::Scale.new(tool_f, :from=>8, :to=>2000, :variable=>@delay,
                           :command=>proc{|n| set_delay(n)})
    check = Ttk::Checkbutton.new(tool_f, :text=>'反復',
                                 :variable=>@continuous)
    @start_btn = Ttk::Button.new(tool_f, :text=>'開始',
                                 :command=>proc{tour()})
    @exit_btn = Ttk::Button.new(tool_f, :text=>'終了',
                                :command=>proc{_exit()})

    7.downto(0){|row|
      0.upto(7){|col|
        if ((col & 1) ^ (row & 1)).zero?
          fill  = 'bisque'
          dfill = 'bisque3'
        else
          fill  = 'tan3'
          dfill = 'tan4'
        end
        coords = [col * 30 + 4, row * 30 + 4, col * 30 + 30, row * 30 + 30]
        @board.create(TkcRectangle, coords,
                      :fill=>fill, :disabledfill=>dfill,
                      :width=>2, :state=>:disabled)
      }
    }

    @knight_font = TkFont.new(:size=>-24)
    @knight = TkcText.new(@board, 0, 0, :font=>@knight_font,
                          :text=>Tk::UTF8_String.new('\u265e'),
                          :anchor=>'nw', # :tags=>'knight',
                          :fill=>'black', :activefill=>'#600000')
    @knight.coords(@board.coords(rand(64)+1)[0..1])
    @knight.bind('ButtonPress-1', '%W %x %y'){|w,x,y| drag_start(w,x,y)}
    @knight.bind('Motion', '%W %x %y'){|w,x,y| drag_motion(w,x,y)}
    @knight.bind('ButtonRelease-1', '%W %x %y'){|w,x,y| drag_end(w,x,y)}

    Tk.grid(@board, @log, scr, :sticky=>'news')
    base_f.grid_rowconfigure(0, :weight=>1)
    base_f.grid_columnconfigure(0, :weight=>1)

    Tk.grid(base_f, '-', '-', '-', '-', '-', :sticky=>'news')
    widgets = [label, scale, check, @start_btn]
    sg = nil
    unless $RubyTk_WidgetDemo
      widgets << @exit_btn
      if Tk.windowingsystem != 'aqua'
        #widgets.unshift(Ttk::SizeGrip.new(tool_f))
        Ttk::SizeGrip.new(tool_f).pack(:side=>:right, :anchor=>'se')
      end
    end
    Tk.pack(widgets, :side=>:right)
    if Tk.windowingsystem == 'aqua'
      TkPack.configure(widgets, :padx=>[4, 4], :pady=>[12, 12])
      TkPack.configure(widgets[0], :padx=>[4, 24])
      TkPack.configure(widgets[-1], :padx=>[16, 4])
    end

    Tk.grid(tool_f, '-', '-', '-', '-', '-', :sticky=>'ew')

    if $RubyTk_WidgetDemo
      Tk.grid(make_SeeDismiss(), '-', '-', '-', '-', '-', :sticky=>'ew')
    end

    $knightstour.grid_rowconfigure(0, :weight=>1)
    $knightstour.grid_columnconfigure(0, :weight=>1)

    $knightstour.bind('Control-F2'){TkConsole.show}
    $knightstour.bind('Return'){@start_btn.invoke}
    $knightstour.bind('Escape'){@exit_btn.invoke}
    $knightstour.bind('Destroy'){ _stop }
    $knightstour.protocol('WM_DELETE_WINDOW'){ _exit }

    $knightstour.deiconify
    $knightstour.tkwait_destroy
  end

  def initialize(parent = nil)
    create_gui(parent)
  end
end

Tk.root.withdraw unless $RubyTk_WidgetDemo
Thread.new{Tk.mainloop} if __FILE__ == $0
Knights_Tour.new
