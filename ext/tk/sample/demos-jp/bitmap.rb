# -*- coding: utf-8 -*-
#
# bitmap widget demo (called by 'widget')
#

# bitmapRow --
# Create a row of bitmap items in a window.
#
# Arguments:
# w -           The parent window that is to contain the row.
# args -        The names of one or more bitmaps, which will be displayed
#               in a new row across the bottom of w along with their
#               names.

def bitmapRow(w,*args)
  TkFrame.new(w){|row|
    pack('side'=>'top', 'fill'=>'both')
    for bitmap in args
      TkFrame.new(row){|base|
        pack('side'=>'left', 'fill'=>'both', 'pady'=>'.25c', 'padx'=>'.25c')
        TkLabel.new(base, 'text'=>bitmap, 'width'=>9).pack('side'=>'bottom')
        Tk::Label.new(base, 'bitmap'=>bitmap).pack('side'=>'bottom')
      }
    end
  }
end

# toplevel widget が存在すれば削除する
if defined?($bitmap_demo) && $bitmap_demo
  $bitmap_demo.destroy
  $bitmap_demo = nil
end

# demo 用の toplevel widget を生成
$bitmap_demo = TkToplevel.new {|w|
  title("Bitmap Demonstration")
  iconname("bitmap")
  positionWindow(w)
}

base_frame = TkFrame.new($bitmap_demo).pack(:fill=>:both, :expand=>true)

# label 生成
TkLabel.new(base_frame,'font'=>$font,'wraplength'=>'4i','justify'=>'left',
            'text'=>"このウィンドウには、Tk に組み込まれたすべてのビットマップが、それらの名前と共に表示されています。Tcl のスクリプト中では、それぞれの名前を用いて参照します。"){
  pack('side'=>'top')
}

# frame 生成
$bitmap_buttons = TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    #text '了解'
    text '閉じる'
    command proc{
      tmppath = $bitmap_demo
      $bitmap_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'コード参照'
    command proc{showCode 'bitmap'}
  }.pack('side'=>'left', 'expand'=>'yes')
}
$bitmap_buttons.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# frame 設定
TkFrame.new(base_frame){|f|
  bitmapRow(f,'error','gray25','gray50','hourglass')
  bitmapRow(f,'info','question','questhead','warning')
  pack('side'=>'top', 'expand'=>'yes', 'fill'=>'both')
}

