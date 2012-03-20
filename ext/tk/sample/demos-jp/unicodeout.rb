# -*- coding: utf-8 -*-
#
# unicodeout.rb --
#
# This demonstration script shows how you can produce output (in label
# widgets) using many different alphabets.
#
# based on Tcl/Tk8.4.4 widget demos

if defined?($unicodeout_demo) && $unicodeout_demo
  $unicodeout_demo.destroy
  $unicodeout_demo = nil
end

$unicodeout_demo = TkToplevel.new {|w|
  title("Unicode Label Demonstration")
  iconname("unicodeout")
  positionWindow(w)
}

base_frame = TkFrame.new($unicodeout_demo).pack(:fill=>:both, :expand=>true)

TkLabel.new(base_frame,
            :font=>$font, :wraplength=>'5.4i', :justify=>:left,
            :text=><<EOL).pack(:side=>:top)
これは，Tkにおける非欧米文字集合を用いる言語に対するサポートについての\
サンプルです．ただし，下の表示においてあなたが実際にどのような表示を\
目にするかは，あなたの環境にどのような文字集合がインストールされているかに\
大きく依存します．また，対象となる文字集合がインストールされていない場合に\
どのような表示がなされるかもあなたの環境次第です．\
「コード参照」ボタンを押してソースを表示し，\
Unicodeout_SampleFrameクラスの@@fontの定義を書き換えて\
(ファイルの内容は変更されません)\
「再実行」ボタンのクリックを試してみてください．
スクリプトが可搬性を持つように，文字列は\\uXXXXという\
Tclのエスケープ表現を用いたUNICODE文字列で書かれています．\
文字列は，Tk::UTF8_Stringメソッドによって，\
「UTF8形式の文字列である」という\
エンコード情報付きの文字列オブジェクト\
(Tclのエスケープ表現の変換済み)に変換して\
ラベルウィジェットに渡している点に注意してください．
EOL
#'

TkFrame.new(base_frame){|f|
  pack(:side=>:bottom, :fill=>:x, :pady=>'2m')

  TkButton.new(f, :text=>'閉じる', :width=>15, :command=>proc{
                 $unicodeout_demo.destroy
                 $unicodeout_demo = nil
               }).pack(:side=>:left, :expand=>true)

  TkButton.new(f, :text=>'コード参照', :width=>15, :command=>proc{
                 showCode 'unicodeout'
               }).pack(:side=>:left, :expand=>true)
}

wait_msg = TkLabel.new(base_frame,
                       :text=>"フォント読み込みの完了まで" +
                              "しばらくお待ち下さい．．．",
                       :font=>"Helvetica 12 italic").pack

class Unicodeout_SampleFrame < TkFrame
  @@font = $font
  # @@font = 'Helvetica 14'
  # @@font = 'Courier 12'
  # @@font = 'clearlyu 16'
  # @@font = 'fixed 12'
  # @@font = 'Times 12'
  # @@font = 'Newspaper 12'
  # @@font = '{New century schoolbook} 12'

  def initialize(base)
    super(base)
    grid_columnconfig(1, :weight=>1)
  end

  def add_sample(lang, *args)
    sample_txt = Tk::UTF8_String(args.join(''))
    l = TkLabel.new(self, :font=>@@font, :text=>lang+':',
                    :anchor=>:nw, :pady=>0)
    #s = TkLabel.new(self, :font=>@@font, :text=>sample_txt,
    s = TkLabel.new(self, :font=>TkFont.new(@@font), :text=>sample_txt,
                    :anchor=>:nw, :width=>30, :pady=>0)
    Tk.grid(l, s, :sticky=>:ew, :pady=>0)
    l.grid_config(:padx, '1m')
  end
end
f = Unicodeout_SampleFrame.new(base_frame)
f.pack(:expand=>true, :fill=>:both, :padx=>'2m', :pady=>'1m')

# Processing when some characters are missing might take a while, so make
# sure we're displaying something in the meantime...

oldCursor = $unicodeout_demo.cursor
$unicodeout_demo.cursor('watch')
Tk.update

f.add_sample('Arabic',
             '\uFE94\uFEF4\uFE91\uFEAE\uFECC\uFEDF\uFE8D\uFE94',
             '\uFEE4\uFEE0\uFEDC\uFEDF\uFE8D')
f.add_sample('Trad. Chinese', '\u4E2D\u570B\u7684\u6F22\u5B57')
f.add_sample('Simpl. Chinese', '\u6C49\u8BED')
f.add_sample('Greek',
             '\u0395\u03BB\u03BB\u03B7\u03BD\u03B9\u03BA\u03AE ',
             '\u03B3\u03BB\u03CE\u03C3\u03C3\u03B1')
f.add_sample('Hebrew',
             '\u05DD\u05D9\u05DC\u05E9\u05D5\u05E8\u05D9 ',
             '\u05DC\u05D9\u05D0\u05E8\u05E9\u05D9')
f.add_sample('Japanese',
             '\u65E5\u672C\u8A9E\u306E\u3072\u3089\u304C\u306A, ',
             '\u6F22\u5B57\u3068\u30AB\u30BF\u30AB\u30CA')
f.add_sample('Korean', '\uB300\uD55C\uBBFC\uAD6D\uC758 \uD55C\uAE00')
f.add_sample('Russian',
             '\u0420\u0443\u0441\u0441\u043A\u0438\u0439 ',
             '\u044F\u0437\u044B\u043A')

wait_msg.destroy
$unicodeout_demo.cursor(oldCursor)
