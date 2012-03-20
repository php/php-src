#! /usr/bin/ruby -Ku
# -*- coding: utf-8 -*-

class Board
  def clr
    print "\e[2J"
  end
  def pos(x,y)
    printf "\e[%d;%dH", y+1, x*2+1
  end
  def colorstr(id,s)
    printf "\e[%dm%s\e[0m", id, s
  end
  def put(x, y, col, str)
    pos(x,y); colorstr(43,str)
    pos(0,@hi); print "残り:",@mc,"/",@total,"   "
    pos(x,y)
  end
  private :clr, :pos, :colorstr, :put
  CHR=["・","１","２","３","４","５","６","７","８","★","●","@@"]
  COL=[46,43,45] # default,opened,over
  def initialize(h,w,m)
    # ゲーム盤の生成(h:縦，w:横，m:爆弾の数)
    @hi=h; @wi=w; @m=m
    reset
  end
  def reset
    # ゲーム盤を(再)初期化する
    srand()
    @cx=0; @cy=0; @mc=@m
    @over=false
    @data=Array.new(@hi*@wi)
    @state=Array.new(@hi*@wi)
    @total=@hi*@wi
    @total.times {|i| @data[i]=0}
    @m.times do
       loop do
         j=rand(@total-1)
         if @data[j] == 0 then
           @data[j]=1
           break
         end
       end
    end
    clr; pos(0,0)
    @hi.times{|y| pos(0,y); colorstr(COL[0],CHR[0]*@wi)}
    pos(@cx,@cy)
  end
  def mark
    # 現在のカーソル位置にマークをつける
    if @state[@wi*@cy+@cx] != nil then return end
    @state[@wi*@cy+@cx] = "MARK"
    @mc=@mc-1;
    @total=@total-1;
    put(@cx, @cy, COL[1], CHR[9])
  end
  def open(x=@cx,y=@cy)
    # 現在のカーソル位置をオープンにする
    # 爆弾があればゲームオーバー
    if @state[@wi*y+x] =="OPEN"  then return 0 end
    if @state[@wi*y+x] == nil then @total=@total-1 end
    if @state[@wi*y+x] =="MARK" then @mc=@mc+1 end
    @state[@wi*y+x]="OPEN"
    if fetch(x,y) == 1 then @over = 1; return end
    c = count(x,y)
    put(x, y, COL[1], CHR[c])
    return 0 if c != 0
    if x > 0 && y > 0         then open(x-1,y-1) end
    if y > 0                  then open(x,  y-1) end
    if x < @wi-1 && y > 0     then open(x+1,y-1) end
    if x > 0                  then open(x-1,y) end
    if x < @wi-1              then open(x+1,y) end
    if x > 0 && y < @hi-1     then open(x-1,y+1) end
    if y < @hi -1             then open(x,y+1) end
    if x < @wi-1 && y < @hi-1 then open(x+1,y+1) end
    pos(@cx,@cy)
  end
  def fetch(x,y)
    # (x,y)の位置の爆弾の数(0 or 1)を返す
    if x < 0 then 0
    elsif x >= @wi then 0
    elsif y < 0 then 0
    elsif y >= @hi then 0
    else
      @data[y*@wi+x]
    end
  end
  def count(x,y)
    # (x,y)に隣接する爆弾の数を返す
    fetch(x-1,y-1)+fetch(x,y-1)+fetch(x+1,y-1)+
    fetch(x-1,y)  +             fetch(x+1,y)+
    fetch(x-1,y+1)+fetch(x,y+1)+fetch(x+1,y+1)
  end
  def over(win)
    # ゲームの終了
    quit
    unless win
      pos(@cx,@cy); print CHR[11]
    end
    pos(0,@hi)
    if win then print "*** YOU WIN !! ***"
    else print "*** GAME OVER ***"
    end
  end
  def over?
    # ゲームの終了チェック
    # 終了処理も呼び出す
    remain = (@mc+@total == 0)
    if @over || remain
      over(remain)
      true
    else
      false
    end
  end
  def quit
    # ゲームの中断(または終了)
    # 盤面を全て見せる
    @hi.times do|y|
      pos(0,y)
      @wi.times do|x|
	colorstr(if @state[y*@wi+x] == "MARK" then COL[1] else COL[2] end,
		 if fetch(x,y)==1 then CHR[10] else CHR[count(x,y)] end)
      end
    end
  end
  def down
    # カーソルを下に
    if @cy < @hi-1 then @cy=@cy+1; pos(@cx, @cy) end
  end
  def up
    # カーソルを上に
    if @cy > 0 then @cy=@cy-1; pos(@cx, @cy) end
  end
  def left
    # カーソルを左に
    if @cx > 0 then @cx=@cx-1; pos(@cx, @cy) end
  end
  def right
    # カーソルを右に
    if @cx < @wi-1 then @cx=@cx+1; pos(@cx, @cy) end
  end
end

bd=Board.new(10,10,10)
system("stty raw -echo")
begin
  loop do
    case STDIN.getc
    when ?n  # new game
      bd.reset
    when ?m  # mark
      bd.mark
    when ?j
      bd.down
    when ?k
      bd.up
    when ?h
      bd.left
    when ?l
      bd.right
    when ?\s
      bd.open
    when ?q,?\C-c  # quit game
      bd.quit
      break
    end
    if bd.over?
      if STDIN.getc == ?q then break end
      bd.reset
    end
  end
ensure
  system("stty -raw echo")
end
print "\n"
