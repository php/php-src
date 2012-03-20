# -*- coding: utf-8 -*-
#
# toolbar.rb --
#
# This demonstration script creates a toolbar that can be torn off.
#
# based on "Id: toolbar.tcl,v 1.3 2007/12/13 15:27:07 dgp Exp"

if defined?($toolbar_demo) && $toolbar_demo
  $toolbar_demo.destroy
  $toolbar_demo = nil
end

$toolbar_demo = TkToplevel.new {|w|
  title("Ttk Menu Buttons")
  iconname("toolbar")
  positionWindow(w)
}

base_frame = Ttk::Frame.new($toolbar_demo).pack(:fill=>:both, :expand=>true)

if Tk.windowingsystem != 'aqua'
  msg = Ttk::Label.new(base_frame, :wraplength=>'4i', :text=><<EOL)
このデモでは，ツールバーをどのようにして適切にテーマ対応させるか，\
また，どのようにして切り離し可能にするかを示しています\
（ただし，ツールバーの切り離しにはTcl/Tk8.5以上の機能が必要です）．\
ツールバーのボタンは，'Toolbutton'スタイルを使用するように明示することで，\
"toolbar style"ボタンとなるように属性設定されています．\
ツールバーの左端には簡単なマーカーが置かれています．\
マーカー上にマウスカーソルが来ると移動アイコンにカーソルが変化します．\
そこでツールバーを動かすようにドラッグすると，\
ツールバー全体を切り離して独立したトップレベルウィジェットに\
することができます．\
切り離したツールバーが不要となった時には，\
一般的なトップレベルウィジェットと同様に単純に閉じることで，
再び元のウィンドウに張り付けられるでしょう．
EOL
else
  msg = Ttk::Label.new(base_frame, :wraplength=>'4i', :text=><<EOL)
このデモでは，ツールバーをどのようにして適切にテーマ対応させるかを\
示しています．\
ツールバーのボタンは，'Toolbutton'スタイルを使用するように明示することで，\
"toolbar style"ボタンとなるように属性設定されています．
EOL
end

## Set up the toolbar hull
tbar_base = Tk::Frame.new(base_frame,  # Tk 標準の frame でなければなりません．
                          :widgetname=>'toolbar') # ウィンドウタイトル文字列とするために，ウィジェット名を明示しています．
sep = Ttk::Separator.new(base_frame)
to_base = Ttk::Frame.new(tbar_base, :cursor=>'fleur')
if Tk.windowingsystem != 'aqua'
  to  = Ttk::Separator.new(to_base, :orient=>:vertical)
  to2 = Ttk::Separator.new(to_base, :orient=>:vertical)
  to.pack(:fill=>:y, :expand=>true, :padx=>2, :side=>:left)
  to2.pack(:fill=>:y, :expand=>true, :side=>:left)
end

contents = Ttk::Frame.new(tbar_base)
Tk.grid(to_base, contents, :sticky=>'nsew')
tbar_base.grid_columnconfigure(contents, :weight=>1)
contents.grid_columnconfigure(1000, :weight=>1)

if Tk.windowingsystem != 'aqua'
  ## Bindings so that the toolbar can be torn off and reattached
  to_base.bind('B1-Motion', '%X %Y'){|x, y| tbar_base.tearoff(to_base, x, y)}
  to.     bind('B1-Motion', '%X %Y'){|x, y| tbar_base.tearoff(to_base, x, y)}
  to2.    bind('B1-Motion', '%X %Y'){|x, y| tbar_base.tearoff(to_base, x, y)}
  def tbar_base.tearoff(w, x, y)
    on_win = TkWinfo.containing(x, y)
    return unless (on_win && on_win.path =~ /^#{@path}(\.|$)/)
    self.grid_remove
    w.grid_remove
    self.wm_manage
    # self.wm_title('Toolbar') # もしウィジェット名をウィンドウタイトルにしたくないなら，ここで設定してください
    self.wm_protocol('WM_DELETE_WINDOW'){ self.untearoff(w) }
  end
  def tbar_base.untearoff(w)
    self.wm_forget
    w.grid
    self.grid
  end
end

## Some content for the rest of the toplevel
text = TkText.new(base_frame, :width=>40, :height=>10)

## Toolbar contents
tb_btn = Ttk::Button.new(tbar_base, :text=>'ボタン', :style=>'Toolbutton',
                         :command=>proc{
                           text.insert(:end, "ボタンが押されました．\n")
                         })
tb_chk = Ttk::Checkbutton.new(tbar_base, :text=>'チェックボタン',
                              :style=>'Toolbutton',
                              :variable=>(check = TkVariable.new),
                              :command=>proc{
                                text.insert(:end, "チェックボタンの値は#{check.value}です．\n")
                              })
tb_mbtn = Ttk::Menubutton.new(tbar_base, :text=>'メニュー')
tb_combo = Ttk::Combobox.new(tbar_base, :value=>TkFont.families,
                             :state=>:readonly)
tb_mbtn.menu(menu = Tk::Menu.new(tb_mbtn))
menu.add(:command, :label=>'Just', :command=>proc{text.insert(:end, "Just\n")})
menu.add(:command, :label=>'An', :command=>proc{text.insert(:end, "An\n")})
menu.add(:command, :label=>'Example',
         :command=>proc{text.insert(:end, "Example\n")})
tb_combo.bind('<ComboboxSelected>'){ text.font.family = tb_combo.get }

## Arrange contents
Tk.grid(tb_btn, tb_chk, tb_mbtn, tb_combo,
        :in=>contents, :padx=>2, :sticky=>'ns')
Tk.grid(tbar_base, :sticky=>'ew')
Tk.grid(sep, :sticky=>'ew')
Tk.grid(msg, :sticky=>'ew')
Tk.grid(text, :sticky=>'nsew')
base_frame.grid_rowconfigure(text, :weight=>1)
base_frame.grid_columnconfigure(text, :weight=>1)

## See Code / Dismiss buttons
Ttk::Frame.new(base_frame) {|frame|
  sep = Ttk::Separator.new(frame)
  Tk.grid(sep, :columnspan=>4, :row=>0, :sticky=>'ew', :pady=>2)
  TkGrid('x',
         Ttk::Button.new(frame, :text=>'コード参照',
                         :image=>$image['view'], :compound=>:left,
                         :command=>proc{showCode 'toolbar'}),
         Ttk::Button.new(frame, :text=>'閉じる',
                         :image=>$image['delete'], :compound=>:left,
                         :command=>proc{
                           $toolbar_demo.destroy
                           $toolbar_demo = nil
                         }),
         :padx=>4, :pady=>4)
  grid_columnconfigure(0, :weight=>1)
  Tk.grid(frame, :sticky=>'ew')
}
