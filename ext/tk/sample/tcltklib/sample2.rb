#!/usr/local/bin/ruby
#----------------------> pretty simple othello game <-----------------------
# othello.rb
#
# version 0.3
# maeda shugo (shuto@po.aianet.ne.jp)
#---------------------------------------------------------------------------

#       Sep. 17, 1997   modified by Y. Shigehiro for tcltk library
#          maeda shugo (shugo@po.aianet.ne.jp) 氏による
#          (ruby/tk で書かれていた) ruby のサンプルプログラム
#               http://www.aianet.or.jp/~shugo/ruby/othello.rb.gz
#          を tcltk ライブラリを使うように, 機械的に変更してみました.
#
#          なるべくオリジナルと同じになるようにしてあります.

require "observer"
require "tcltk"
$ip = TclTkInterpreter.new()
$root = $ip.rootwidget()
$button, $canvas, $checkbutton, $frame, $label, $pack, $update, $wm =
   $ip.commands().values_at(
   "button", "canvas", "checkbutton", "frame", "label", "pack", "update", "wm")

class Othello

   EMPTY = 0
   BLACK = 1
   WHITE = - BLACK

   attr :in_com_turn
   attr :game_over

   class Board

      include Observable

      DIRECTIONS = [
         [-1, -1], [-1, 0], [-1, 1],
         [ 0, -1],          [ 0, 1],
         [ 1, -1], [ 1, 0], [ 1, 1]
      ]

      attr_accessor :com_disk

      def initialize(othello)
         @othello = othello
         reset
      end

      def notify_observers(*arg)
         if @observer_peers != nil
            super(*arg)
         end
      end

      def reset
         @data = [
            [EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY],
            [EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY],
            [EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY],
            [EMPTY, EMPTY, EMPTY, WHITE, BLACK, EMPTY, EMPTY, EMPTY],
            [EMPTY, EMPTY, EMPTY, BLACK, WHITE, EMPTY, EMPTY, EMPTY],
            [EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY],
            [EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY],
            [EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY]
         ]
         changed
         notify_observers
      end

      def man_disk
         return - @com_disk
      end

      def other_disk(disk)
         return - disk
      end

      def get_disk(row, col)
         return @data[row][col]
      end

      def reverse_to(row, col, my_disk, dir_y, dir_x)
         y = row
         x = col
         begin
            y += dir_y
            x += dir_x
            if y < 0 || x < 0 || y > 7 || x > 7 ||
                  @data[y][x] == EMPTY
               return
            end
         end until @data[y][x] == my_disk
         begin
            @data[y][x] = my_disk
            changed
            notify_observers(y, x)
            y -= dir_y
            x -= dir_x
         end until y == row && x == col
      end

      def put_disk(row, col, disk)
         @data[row][col] = disk
         changed
         notify_observers(row, col)
         DIRECTIONS.each do |dir|
            reverse_to(row, col, disk, *dir)
         end
      end

      def count_disk(disk)
         num = 0
         @data.each do |rows|
            rows.each do |d|
               if d == disk
                  num += 1
               end
            end
         end
         return num
      end

      def count_point_to(row, col, my_disk, dir_y, dir_x)
         return 0 if @data[row][col] != EMPTY
         count = 0
         loop do
            row += dir_y
            col += dir_x
            break if row < 0 || col < 0 || row > 7 || col > 7
            case @data[row][col]
            when my_disk
               return count
            when other_disk(my_disk)
               count += 1
            when EMPTY
               break
            end
         end
         return 0
      end

      def count_point(row, col, my_disk)
         count = 0
         DIRECTIONS.each do |dir|
            count += count_point_to(row, col, my_disk, *dir)
         end
         return count
      end

      def corner?(row, col)
         return (row == 0 && col == 0) ||
            (row == 0 && col == 7) ||
            (row == 7 && col == 0) ||
            (row == 7 && col == 7)
      end

      def search(my_disk)
         max = 0
         max_row = nil
         max_col = nil
         for row in 0 .. 7
            for col in 0 .. 7
               buf = count_point(row, col, my_disk)
               if (corner?(row, col) && buf > 0) || max < buf
                  max = buf
                  max_row = row
                  max_col = col
               end
            end
         end
         return max_row, max_col
      end
   end #--------------------------> class Board ends here

   class BoardView < TclTkWidget

      BACK_GROUND_COLOR = "DarkGreen"
      HILIT_BG_COLOR = "green"
      BORDER_COLOR = "black"
      BLACK_COLOR = "black"
      WHITE_COLOR = "white"
      STOP_COLOR = "red"

      attr :left
      attr :top
      attr :right
      attr :bottom

      class Square

         attr :oval, TRUE
         attr :row
         attr :col

         def initialize(view, row, col)
            @view = view
            @id = @view.e("create rectangle",
                          *(view.tk_rect(view.left + col,
                                         view.top + row,
                                         view.left + col + 1,
                                         view.top + row + 1) \
                            << "-fill #{BACK_GROUND_COLOR}") )
            @row = row
            @col = col
            @view.e("itemconfigure", @id,
              "-width 0.5m -outline #{BORDER_COLOR}")
            @view.e("bind", @id, "<Any-Enter>", TclTkCallback.new($ip, proc{
               if @oval == nil
                  view.e("itemconfigure", @id, "-fill #{HILIT_BG_COLOR}")
               end
            }))
            @view.e("bind", @id, "<Any-Leave>", TclTkCallback.new($ip, proc{
               view.e("itemconfigure", @id, "-fill #{BACK_GROUND_COLOR}")
            }))
            @view.e("bind", @id, "<ButtonRelease-1>", TclTkCallback.new($ip,
               proc{
               view.click_square(self)
            }))
         end

         def blink(color)
            @view.e("itemconfigure", @id, "-fill #{color}")
            $update.e()
            sleep(0.1)
            @view.e("itemconfigure", @id, "-fill #{BACK_GROUND_COLOR}")
         end
      end #-----------------------> class Square ends here

      def initialize(othello, board)
         super($ip, $root, $canvas)
         @othello = othello
         @board = board
         @board.add_observer(self)

         @squares = Array.new(8)
         for i in 0 .. 7
            @squares[i] = Array.new(8)
         end
         @left = 1
         @top = 0.5
         @right = @left + 8
         @bottom = @top + 8

         i = self.e("create rectangle", *tk_rect(@left, @top, @right, @bottom))
         self.e("itemconfigure", i,
            "-width 1m -outline #{BORDER_COLOR} -fill #{BACK_GROUND_COLOR}")

         for row in 0 .. 7
            for col in 0 .. 7
               @squares[row][col] = Square.new(self, row, col)
            end
         end

         update
      end

      def tk_rect(left, top, right, bottom)
         return left.to_s + "c", top.to_s + "c",
            right.to_s + "c", bottom.to_s + "c"
      end

      def clear
         each_square do |square|
            if square.oval != nil
               self.e("delete", square.oval)
               square.oval = nil
            end
         end
      end

      def draw_disk(row, col, disk)
         if disk == EMPTY
            if @squares[row][col].oval != nil
               self.e("delete", @squares[row][col].oval)
               @squares[row][col].oval = nil
            end
            return
         end

         $update.e()
         sleep(0.05)
         oval = @squares[row][col].oval
         if oval == nil
            oval = self.e("create oval", *tk_rect(@left + col + 0.2,
                                           @top + row + 0.2,
                                           @left + col + 0.8,
                                           @top + row + 0.8))
            @squares[row][col].oval = oval
         end
         case disk
         when BLACK
            color = BLACK_COLOR
         when WHITE
            color = WHITE_COLOR
         else
            fail format("Unknown disk type: %d", disk)
         end
         self.e("itemconfigure", oval, "-outline #{color} -fill #{color}")
      end

      def update(row = nil, col = nil)
         if row && col
            draw_disk(row, col, @board.get_disk(row, col))
         else
            each_square do |square|
               draw_disk(square.row, square.col,
                         @board.get_disk(square.row, square.col))
            end
         end
         @othello.show_point
      end

      def each_square
         @squares.each do |rows|
            rows.each do |square|
               yield(square)
            end
         end
      end

      def click_square(square)
         if @othello.in_com_turn || @othello.game_over ||
               @board.count_point(square.row,
                                  square.col,
                                  @board.man_disk) == 0
            square.blink(STOP_COLOR)
            return
         end
         @board.put_disk(square.row, square.col, @board.man_disk)
         @othello.com_turn
      end

      private :draw_disk
      public :update
   end #----------------------> class BoardView ends here

   def initialize
      @msg_label = TclTkWidget.new($ip, $root, $label)
      $pack.e(@msg_label)

      @board = Board.new(self)
      @board_view = BoardView.new(self, @board)
      #### added by Y. Shigehiro
      ## board_view の大きさを設定する.
      x1, y1, x2, y2 = @board_view.e("bbox all").split(/ /).collect{|i| i.to_f}
      @board_view.e("configure -width", x2 - x1)
      @board_view.e("configure -height", y2 - y1)
      ## scrollregion を設定する.
      @board_view.e("configure -scrollregion {", @board_view.e("bbox all"),
         "}")
      #### ここまで
      $pack.e(@board_view, "-fill both -expand true")

      panel = TclTkWidget.new($ip, $root, $frame)

      @play_black = TclTkWidget.new($ip, panel, $checkbutton,
        "-text {com is black} -command", TclTkCallback.new($ip, proc{
         switch_side
      }))
      $pack.e(@play_black, "-side left")

      quit = TclTkWidget.new($ip, panel, $button, "-text Quit -command",
         TclTkCallback.new($ip, proc{
         exit
      }))
      $pack.e(quit, "-side right -fill x")

      reset = TclTkWidget.new($ip, panel, $button, "-text Reset -command",
         TclTkCallback.new($ip, proc{
         reset_game
      }))
      $pack.e(reset, "-side right -fill x")

      $pack.e(panel, "-side bottom -fill x")

#      root = Tk.root
      $wm.e("title", $root, "Othello")
      $wm.e("iconname", $root, "Othello")

      @board.com_disk = WHITE
      @game_over = FALSE

      TclTk.mainloop
   end

   def switch_side
      if @in_com_turn
         @play_black.e("toggle")
      else
         @board.com_disk = @board.man_disk
         com_turn unless @game_over
      end
   end

   def reset_game
      if @board.com_disk == BLACK
         @board.com_disk = WHITE
         @play_black.e("toggle")
      end
      @board_view.clear
      @board.reset
      $wm.e("title", $root, "Othello")
      @game_over = FALSE
   end

   def com_turn
      @in_com_turn = TRUE
      $update.e()
      sleep(0.5)
      begin
         com_disk = @board.count_disk(@board.com_disk)
         man_disk = @board.count_disk(@board.man_disk)
         if @board.count_disk(EMPTY) == 0
            if man_disk == com_disk
               $wm.e("title", $root, "{Othello - Draw!}")
            elsif man_disk > com_disk
               $wm.e("title", $root, "{Othello - You Win!}")
            else
               $wm.e("title", $root, "{Othello - You Loose!}")
            end
            @game_over = TRUE
            break
         elsif com_disk == 0
            $wm.e("title", $root, "{Othello - You Win!}")
            @game_over = TRUE
            break
         elsif man_disk == 0
            $wm.e("title", $root, "{Othello - You Loose!}")
            @game_over = TRUE
            break
         end
         row, col = @board.search(@board.com_disk)
         break if row == nil || col == nil
         @board.put_disk(row, col, @board.com_disk)
      end while @board.search(@board.man_disk) == [nil, nil]
      @in_com_turn = FALSE
   end

   def show_point
      black = @board.count_disk(BLACK)
      white = @board.count_disk(WHITE)
      @msg_label.e("configure -text",
         %Q/{#{format("BLACK: %.2d    WHITE: %.2d", black, white)}}/)
   end
end #----------------------> class Othello ends here

Othello.new

#----------------------------------------------> othello.rb ends here
