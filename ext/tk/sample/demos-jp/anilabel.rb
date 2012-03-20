# -*- coding: utf-8 -*-
#
# animated label widget demo (called by 'widget')
#
# based on Tcl/Tk8.5a2 widget demos

# toplevel widget が存在すれば削除する
if defined?($anilabel_demo) && $anilabel_demo
  $anilabel_demo.destroy
  $anilabel_demo = nil
end

# demo 用の toplevel widget を生成
$anilabel_demo = TkToplevel.new {|w|
  title("Animated Label Demonstration")
  iconname("anilabel")
  positionWindow(w)
}

base_frame = TkFrame.new($anilabel_demo).pack(:fill=>:both, :expand=>true)

# label 生成
msg = TkLabel.new(base_frame) {
  font $font
  wraplength '4i'
  justify 'left'
  text "下には4つのアニメーションラベルが表示されています。左側にあるラベルは、内部のテキストメッセージをスクロールしたように見せることで動きを付けています。右側のラベルは、表示するイメージを変化させることで動きを与えています。"
}
msg.pack('side'=>'top')

# frame 生成
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    #text '了解'
    text '閉じる'
    command proc{
      tmppath = $anilabel_demo
      $anilabel_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'コード参照'
    command proc{showCode 'anilabel'}
  }.pack('side'=>'left', 'expand'=>'yes')

}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# label demo 用フレーム生成
f_left = TkLabelFrame.new(base_frame,  :text=>'Scrolling Texts')
f_right = TkLabelFrame.new(base_frame, :text=>'GIF Image')
Tk.pack(f_left, f_right, 'side'=>'left', 'expand'=>'yes', 'fill'=>'both',
        'padx'=>10, 'pady'=>10)

# animated label
class AnimatedTextLabel < TkLabel
  def initialize(*args)
    super(*args)
    @timer = TkTimer.new{ _animation_callback }
    @timer.loop_exec = -1
    # bind('Destroy'){ @timer.stop }
    @btag = TkBindTag.new('Destroy'){ @timer.stop }
    self.bindtags_unshift(@btag)
  end

  def _animation_callback()
    txt = self.text
    self.text = (txt[1..-1] << txt[0])
  end
  private :_animation_callback

  def start(interval)
    @timer.set_interval(interval)
    @timer.start
  end

  def stop
    @timer.stop
  end
end

# animated image
class AnimatedImageLabel < AnimatedTextLabel
  def initialize(*args)
    super(*args)
    @destroy_image = false
    @btag.bind_append('Destroy'){
      if @destroy_image
        begin
          self.image.delete
        rescue
        end
      end
    }
  end
  attr_accessor :destroy_image

  def _animation_callback()
    img = self.image

    fmt = img.format
    if fmt.kind_of?(Array)
      if fmt[1].kind_of?(Hash)
        # fmt == ['GIF', {'index'=>idx}]
        idx = fmt[1]['index']
      else
        # fmt == ['GIF', '-index', idx]  :: Ruby1.8.2 returns this.
        idx = fmt[2]
      end
    elsif fmt.kind_of?(String) && fmt =~ /GIF -index (\d+)/
      idx = $1.to_i
    else
      idx = -1
    end

    begin
      img.format("GIF -index #{idx + 1}")
    rescue => e
      img.format("GIF -index 0")
    end
  end
  private :_animation_callback
end

# label 生成
l1 = AnimatedTextLabel.new(f_left, :borderwidth=>4, :relief=>:ridge,
                           :font=>{:family=>'Courier', :size=>10})
l2 = AnimatedTextLabel.new(f_left, :borderwidth=>4, :relief=>:groove,
                           :font=>{:family=>'Courier', :size=>10})
l3 = AnimatedTextLabel.new(f_left, :borderwidth=>4, :relief=>:flat,
                           :font=>{:family=>'Courier', :size=>10}, :width=>18)
Tk.pack(l1, l2, l3,
        :side=>:top, :expand=>true, :anchor=>:w, :padx=>10, :pady=>10)

limg = AnimatedImageLabel.new(f_right, :borderwidth=>0)
limg.pack(:side=>:top, :expand=>true, :padx=>10, :pady=>10)

# base64-encoded animated GIF file
tclPowerdData = <<EOD
    R0lGODlhKgBAAPQAAP//////zP//AP/MzP/Mmf/MAP+Zmf+ZZv+ZAMz//8zM
    zMyZmcyZZsxmZsxmAMwzAJnMzJmZzJmZmZlmmZlmZplmM5kzM2aZzGZmzGZm
    mWZmZmYzZmYzMzNmzDMzZgAzmSH+IE1hZGUgd2l0aCBHSU1QIGJ5IExARGVt
    YWlsbHkuY29tACH5BAVkAAEALAAAAAAqAEAAAAX+YCCOZEkyTKM2jOm66yPP
    dF03bx7YcuHIDkGBR7SZeIyhTID4FZ+4Es8nQyCe2EeUNJ0peY2s9mi7PhAM
    ngEAMGRbUpvzSxskLh1J+Hkg134OdDIDEB+GHxtYMEQMTjMGEYeGFoomezaC
    DZGSHFmLXTQKkh8eNQVpZ2afmDQGHaOYSoEyhhcklzVmMpuHnaZmDqiGJbg0
    qFqvh6UNAwB7VA+OwydEjgujkgrPNhbTI8dFvNgEYcHcHx0lB1kX2IYeA2G6
    NN0YfkXJ2BsAMuAzHB9cZMk3qoEbRzUACsRCUBK5JxsC3iMiKd8GN088SIyT
    0RAFSROyeEg38caDiB/+JEgqxsODrZJ1BkT0oHKSmI0ceQxo94HDpg0qsuDk
    UmRAMgu8OgwQ+uIJgUMVeGXA+IQkzEeHGvD8cIGlDXsLiRjQ+EHroQhea7xY
    8IQBSgYYDi1IS+OFBCgaDMGVS3fGi5BPJpBaENdQ0EomKGD56IHwO39EXiSC
    Ysgxor5+Xfgq0qByYUpiXmwuoredB2aYH4gWWda0B7SeNENpEJHC1ghi+pS4
    AJpIAwWvKPBi+8YEht5EriEqpFfMlhEdkBNpx0HUhwypx5T4IB1MBg/Ws2sn
    wV3MSQOkzI8fUd48Aw3dOZto71x85hHtHijYv18Gf/3GqCdDCXHNoICBobSo
    IqBqJLyCoH8JPrLgdh88CKCFD0CGmAiGYPgffwceZh6FC2ohIIklnkhehTNY
    4CIHHGzgwYw01ujBBhvAqKOLLq5AAk9kuSPkkKO40NB+h1gnypJIIvkBf09a
    N5QIRz5p5ZJXJpmlIVhOGQA2TmIJZZhKKmmll2BqyWSXWUrZpQtpatlmk1c2
    KaWRHeTZEJF8SqLDn/hhsOeQgBbqAh6DGqronxeARUIIACH5BAUeAAAALAUA
    LgAFAAUAAAUM4CeKz/OV5YmqaRkCACH5BAUeAAEALAUALgAKAAUAAAUUICCK
    z/OdJVCaa7p+7aOWcDvTZwgAIfkEBR4AAQAsCwAuAAkABQAABRPgA4zP95zA
    eZqoWqqpyqLkZ38hACH5BAUKAAEALAcALgANAA4AAAU7ICA+jwiUJEqeKau+
    r+vGaTmac63v/GP9HM7GQyx+jsgkkoRUHJ3Qx0cK/VQVTKtWwbVKn9suNunc
    WkMAIfkEBQoAAAAsBwA3AAcABQAABRGgIHzk842j+Yjlt5KuO8JmCAAh+QQF
    CgAAACwLADcABwAFAAAFEeAnfN9TjqP5oOWziq05lmUIACH5BAUKAAAALA8A
    NwAHAAUAAAUPoPCJTymS3yiQj4qOcPmEACH5BAUKAAAALBMANwAHAAUAAAUR
    oCB+z/MJX2o+I2miKimiawgAIfkEBQoAAAAsFwA3AAcABQAABRGgIHzfY47j
    Q4qk+aHl+pZmCAAh+QQFCgAAACwbADcABwAFAAAFEaAgfs/zCV9qPiNJouo7
    ll8IACH5BAUKAAAALB8ANwADAAUAAAUIoCB8o0iWZggAOw==
EOD

l1.text('* Slow Animation *').start(300)
l2.text('* Fast Animation *').start(80)
l3.text('This is a longer scrolling text in a widget that will not show the whole message at once. ').start(150)

limg.destroy_image = true
limg.image(TkPhotoImage.new(:format=>'GIF', :data=>tclPowerdData)).start(100)
