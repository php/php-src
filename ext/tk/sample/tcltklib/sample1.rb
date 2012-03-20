#! /usr/local/bin/ruby -d
# -*- encoding: utf-8 -*-
# -d オプションを付けると, デバッグ情報を表示する.

# tcltk ライブラリのサンプル

# まず, ライブラリを require する.
require "tcltk"

# 以下は, Test1 のインスタンスの initialize() で,
# tcl/tk に関する処理を行う例である.
# 必ずしもそのようにする必要は無く,
# (もし, そうしたければ) class の外で tcl/tk に関する処理を行っても良い.

class Test1
  # 初期化(インタプリタを生成してウィジェットを生成する).
  def initialize()

    #### 使う前のおまじない

    # インタプリタの生成.
    ip = TclTkInterpreter.new()
    # コマンドに対応するオブジェクトを c に設定しておく.
    c = ip.commands()
    # 使用するコマンドに対応するオブジェクトは変数に入れておく.
    append, bind, button, destroy, incr, info, label, place, set, wm =
      c.values_at(
      "append", "bind", "button", "destroy", "incr", "info", "label", "place",
      "set", "wm")

    #### tcl/tk のコマンドに対応するオブジェクト(TclTkCommand)の操作

    # 実行する時は, e() メソッドを使う.
    # (以下は, tcl/tk における info command r* を実行.)
    print info.e("command", "r*"), "\n"
    # 引数は, まとめた文字列にしても同じ.
    print info.e("command r*"), "\n"
    # 変数を用いなくとも実行できるが, 見ためが悪い.
    print c["info"].e("command", "r*"), "\n"
    # インタプリタのメソッドとしても実行できるが, 効率が悪い.
    print ip.info("command", "r*"), "\n"

    ####

    # 以下, 生成したオブジェクトは変数に代入しておかないと
    # GC の対象になってしまう.

    #### tcl/tk の変数に対応するオブジェクト(TclTkVariable)の操作

    # 生成と同時に値を設定する.
    v1 = TclTkVariable.new(ip, "20")
    # 読み出しは get メソッドを使う.
    print v1.get(), "\n"
    # 設定は set メソッドを使う.
    v1.set(40)
    print v1.get(), "\n"
    # set コマンドを使って読み出し, 設定は可能だが見ためが悪い.
    # e() メソッド等の引数に直接 TclTkObject や数値を書いても良い.
    set.e(v1, 30)
    print set.e(v1), "\n"
    # tcl/tk のコマンドで変数を操作できる.
    incr.e(v1)
    print v1.get(), "\n"
    append.e(v1, 10)
    print v1.get(), "\n"

    #### tcl/tk のウィジェットに対応するオブジェクト(TclTkWidget)の操作

    # ルートウィジェットを取り出す.
    root = ip.rootwidget()
    # ウィジェットの操作.
    root.e("configure -height 300 -width 300")
    # タイトルを付けるときは wm を使う.
    wm.e("title", root, $0)
    # 親ウィジェットとコマンドを指定して, ウィジェットを作る.
    l1 = TclTkWidget.new(ip, root, label, "-text {type `x' to print}")
    # place すると表示される.
    place.e(l1, "-x 0 -rely 0.0 -relwidth 1 -relheight 0.1")
    # コマンド名は文字列で指定しても良いが, 見ためが悪い.
    # (コマンド名は独立した引数でなければならない.)
    l2 = TclTkWidget.new(ip, root, "label")
    # ウィジェットの操作.
    l2.e("configure -text {type `q' to exit}")
    place.e(l2, "-x 0 -rely 0.1 -relwidth 1 -relheight 0.1")

    #### tcl/tk のコールバックに対応するオブジェクト(TclTkCallback)の操作

    # コールバックを生成する.
    c1 = TclTkCallback.new(ip, proc{sample(ip, root)})
    # コールバックを持つウィジェットを生成する.
    b1 = TclTkWidget.new(ip, root, button, "-text sample -command", c1)
    place.e(b1, "-x 0 -rely 0.2 -relwidth 1 -relheight 0.1")
    # イベントループを抜けるには destroy.e(root) する.
    c2 = TclTkCallback.new(ip, proc{destroy.e(root)})
    b2 = TclTkWidget.new(ip, root, button, "-text exit -command", c2)
    place.e(b2, "-x 0 -rely 0.3 -relwidth 1 -relheight 0.1")

    #### イベントのバインド
    # script の追加 (bind tag sequence +script) は今のところできない.
    # (イテレータ変数の設定がうまくいかない.)

    # 基本的にはウィジェットに対するコールバックと同じ.
    c3 = TclTkCallback.new(ip, proc{print("q pressed\n"); destroy.e(root)})
    bind.e(root, "q", c3)
    # bind コマンドで % 置換によりパラメータを受け取りたいときは,
    # proc{} の後ろに文字列で指定すると,
    # 置換結果をイテレータ変数を通して受け取ることができる.
    # ただし proc{} の後ろの文字列は,
    # bind コマンドに与えるコールバック以外で指定してはいけない.
    c4 = TclTkCallback.new(ip, proc{|i| print("#{i} pressed\n")}, "%A")
    bind.e(root, "x", c4)
    # TclTkCallback を GC の対象にしたければ,
    # dcb() (または deletecallbackkeys()) する必要がある.
    cb = [c1, c2, c3, c4]
    c5 = TclTkCallback.new(ip, proc{|w| TclTk.dcb(cb, root, w)}, "%W")
    bind.e(root, "<Destroy>", c5)
    cb.push(c5)

    #### tcl/tk のイメージに対応するオブジェクト(TclTkImage)の操作

    # データを指定して生成する.
    i1 = TclTkImage.new(ip, "photo", "-file maru.gif")
    # ラベルに張り付けてみる.
    l3 = TclTkWidget.new(ip, root, label, "-relief raised -image", i1)
    place.e(l3, "-x 0 -rely 0.4 -relwidth 0.2 -relheight 0.2")
    # 空のイメージを生成して後で操作する.
    i2 = TclTkImage.new(ip, "photo")
    # イメージを操作する.
    i2.e("copy", i1)
    i2.e("configure -gamma 0.5")
    l4 = TclTkWidget.new(ip, root, label, "-relief raised -image", i2)
    place.e(l4, "-relx 0.2 -rely 0.4 -relwidth 0.2 -relheight 0.2")

    ####
  end

  # サンプルのためのウィジェットを生成する.
  def sample(ip, parent)
    bind, button, destroy, grid, toplevel, wm = ip.commands().values_at(
      "bind", "button", "destroy", "grid", "toplevel", "wm")

    ## toplevel

    # 新しいウインドウを開くには, toplevel を使う.
    t1 = TclTkWidget.new(ip, parent, toplevel)
    # タイトルを付けておく
    wm.e("title", t1, "sample")

    # ウィジェットが破壊されたとき, コールバックが GC の対象になるようにする.
    cb = []
    cb.push(c = TclTkCallback.new(ip, proc{|w| TclTk.dcb(cb, t1, w)}, "%W"))
    bind.e(t1, "<Destroy>", c)

    # ボタンの生成.
    wid = []
    # toplevel ウィジェットを破壊するには destroy する.
    cb.push(c = TclTkCallback.new(ip, proc{destroy.e(t1)}))
    wid.push(TclTkWidget.new(ip, t1, button, "-text close -command", c))
    cb.push(c = TclTkCallback.new(ip, proc{test_label(ip, t1)}))
    wid.push(TclTkWidget.new(ip, t1, button, "-text label -command", c))
    cb.push(c = TclTkCallback.new(ip, proc{test_button(ip, t1)}))
    wid.push(TclTkWidget.new(ip, t1, button, "-text button -command", c))
    cb.push(c = TclTkCallback.new(ip, proc{test_checkbutton(ip, t1)}))
    wid.push(TclTkWidget.new(ip, t1, button, "-text checkbutton -command", c))
    cb.push(c = TclTkCallback.new(ip, proc{test_radiobutton(ip, t1)}))
    wid.push(TclTkWidget.new(ip, t1, button, "-text radiobutton -command", c))
    cb.push(c = TclTkCallback.new(ip, proc{test_scale(ip, t1)}))
    wid.push(TclTkWidget.new(ip, t1, button, "-text scale -command", c))
    cb.push(c = TclTkCallback.new(ip, proc{test_entry(ip, t1)}))
    wid.push(TclTkWidget.new(ip, t1, button, "-text entry -command", c))
    cb.push(c = TclTkCallback.new(ip, proc{test_text(ip, t1)}))
    wid.push(TclTkWidget.new(ip, t1, button, "-text text -command", c))
    cb.push(c = TclTkCallback.new(ip, proc{test_raise(ip, t1)}))
    wid.push(TclTkWidget.new(ip, t1, button, "-text raise/lower -command", c))
    cb.push(c = TclTkCallback.new(ip, proc{test_modal(ip, t1)}))
    wid.push(TclTkWidget.new(ip, t1, button, "-text message/modal -command",
      c))
    cb.push(c = TclTkCallback.new(ip, proc{test_menu(ip, t1)}))
    wid.push(TclTkWidget.new(ip, t1, button, "-text menu -command", c))
    cb.push(c = TclTkCallback.new(ip, proc{test_listbox(ip, t1)}))
    wid.push(TclTkWidget.new(ip, t1, button, "-text listbox/scrollbar",
      "-command", c))
    cb.push(c = TclTkCallback.new(ip, proc{test_canvas(ip, t1)}))
    wid.push(TclTkWidget.new(ip, t1, button, "-text canvas -command", c))

    # grid で表示する.
    ro = co = 0
    wid.each{|w|
      grid.e(w, "-row", ro, "-column", co, "-sticky news")
      ro += 1
      if ro == 7
        ro = 0
        co += 1
      end
    }
  end

  # inittoplevel(ip, parent, title)
  #   以下の処理をまとめて行う.
  #       1. toplevel ウィジェットを作成する.
  #       2. コールバックを登録する配列を用意し, toplevel ウィジェットの
  #         <Destroy> イベントにコールバックを削除する手続きを登録する.
  #       3. クローズボタンを作る.
  #     作成した toplevel ウィジェット, クローズボタン, コールバック登録用変数
  #     を返す.
  #   ip: インタプリタ
  #   parent: 親ウィジェット
  #   title: toplevel ウィジェットのウインドウのタイトル
  def inittoplevel(ip, parent, title)
    bind, button, destroy, toplevel, wm = ip.commands().values_at(
      "bind", "button", "destroy", "toplevel", "wm")

    # 新しいウインドウを開くには, toplevel を使う.
    t1 = TclTkWidget.new(ip, parent, toplevel)
    # タイトルを付けておく
    wm.e("title", t1, title)

    # ウィジェットが破壊されたとき, コールバックが GC の対象になるようにする.
    cb = []
    cb.push(c = TclTkCallback.new(ip, proc{|w| TclTk.dcb(cb, t1, w)}, "%W"))
    bind.e(t1, "<Destroy>", c)
    # close ボタンを作っておく.
    # toplevel ウィジェットを破壊するには destroy する.
    cb.push(c = TclTkCallback.new(ip, proc{destroy.e(t1)}))
    b1 = TclTkWidget.new(ip, t1, button, "-text close -command", c)

    return t1, b1, cb
  end

  # label のサンプル.
  def test_label(ip, parent)
    button, global, label, pack = ip.commands().values_at(
      "button", "global", "label", "pack")
    t1, b1, cb = inittoplevel(ip, parent, "label")

    ## label

    # いろいろな形のラベル.
    l1 = TclTkWidget.new(ip, t1, label, "-text {default(flat)}")
    l2 = TclTkWidget.new(ip, t1, label, "-text raised -relief raised")
    l3 = TclTkWidget.new(ip, t1, label, "-text sunken -relief sunken")
    l4 = TclTkWidget.new(ip, t1, label, "-text groove -relief groove")
    l5 = TclTkWidget.new(ip, t1, label, "-text ridge -relief ridge")
    l6 = TclTkWidget.new(ip, t1, label, "-bitmap error")
    l7 = TclTkWidget.new(ip, t1, label, "-bitmap questhead")

    # pack しても表示される.
    pack.e(b1, l1, l2, l3, l4, l5, l6, l7, "-pady 3")

    ## -textvariable

    # tcltk ライブラリの実装では, コールバックは tcl/tk の``手続き''を通して
    # 呼ばれる. したがって, コールバックの中で(大域)変数にアクセスするときは,
    # global する必要がある.
    # global する前に変数に値を設定してしまうとエラーになるので,
    # tcl/tk における表現形だけ生成して, 実際に値を設定しないように,
    # 2 番目の引数には nil を与える.
    v1 = TclTkVariable.new(ip, nil)
    global.e(v1)
    v1.set(100)
    # -textvariable で変数を設定する.
    l6 = TclTkWidget.new(ip, t1, label, "-textvariable", v1)
    # コールバックの中から変数を操作する.
    cb.push(c = TclTkCallback.new(ip, proc{
      global.e(v1); v1.set(v1.get().to_i + 10)}))
    b2 = TclTkWidget.new(ip, t1, button, "-text +10 -command", c)
    cb.push(c = TclTkCallback.new(ip, proc{
      global.e(v1); v1.set(v1.get().to_i - 10)}))
    b3 = TclTkWidget.new(ip, t1, button, "-text -10 -command", c)
    pack.e(l6, b2, b3)
  end

  # button のサンプル.
  def test_button(ip, parent)
    button, pack = ip.commands().values_at("button", "pack")
    t1, b1, cb = inittoplevel(ip, parent, "button")

    ## button

    # コールバック内で参照する変数は先に宣言しておかなければならない.
    b3 = b4 = nil
    cb.push(c = TclTkCallback.new(ip, proc{b3.e("flash"); b4.e("flash")}))
    b2 = TclTkWidget.new(ip, t1, button, "-text flash -command", c)
    cb.push(c = TclTkCallback.new(ip, proc{b2.e("configure -state normal")}))
    b3 = TclTkWidget.new(ip, t1, button, "-text normal -command", c)
    cb.push(c = TclTkCallback.new(ip, proc{b2.e("configure -state disabled")}))
    b4 = TclTkWidget.new(ip, t1, button, "-text disable -command", c)
    pack.e(b1, b2, b3, b4)
  end

  # checkbutton のサンプル.
  def test_checkbutton(ip, parent)
    checkbutton, global, pack = ip.commands().values_at(
      "checkbutton", "global", "pack")
    t1, b1, cb = inittoplevel(ip, parent, "checkbutton")

    ## checkbutton

    v1 = TclTkVariable.new(ip, nil)
    global.e(v1)
    # -variable で変数を設定する.
    ch1 = TclTkWidget.new(ip, t1, checkbutton, "-onvalue on -offvalue off",
      "-textvariable", v1, "-variable", v1)
    pack.e(b1, ch1)
  end

  # radiobutton のサンプル.
  def test_radiobutton(ip, parent)
    global, label, pack, radiobutton = ip.commands().values_at(
      "global", "label", "pack", "radiobutton")
    t1, b1, cb = inittoplevel(ip, parent, "radiobutton")

    ## radiobutton

    v1 = TclTkVariable.new(ip, nil)
    global.e(v1)
    # ヌルストリングは "{}" で指定する.
    v1.set("{}")
    l1 = TclTkWidget.new(ip, t1, label, "-textvariable", v1)
    # -variable で同じ変数を指定すると同じグループになる.
    ra1 = TclTkWidget.new(ip, t1, radiobutton,
      "-text radio1 -value r1 -variable", v1)
    ra2 = TclTkWidget.new(ip, t1, radiobutton,
      "-text radio2 -value r2 -variable", v1)
    cb.push(c = TclTkCallback.new(ip, proc{global.e(v1); v1.set("{}")}))
    ra3 = TclTkWidget.new(ip, t1, radiobutton,
      "-text clear -value r3 -variable", v1, "-command", c)
    pack.e(b1, l1, ra1, ra2, ra3)
  end

  # scale のサンプル.
  def test_scale(ip, parent)
    global, pack, scale = ip.commands().values_at(
      "global", "pack", "scale")
    t1, b1, cb = inittoplevel(ip, parent, "scale")

    ## scale

    v1 = TclTkVariable.new(ip, nil)
    global.e(v1)
    v1.set(219)
    # コールバック内で参照する変数は先に宣言しておかなければならない.
    sca1 = nil
    cb.push(c = TclTkCallback.new(ip, proc{global.e(v1); v = v1.get();
      sca1.e("configure -background", format("#%02x%02x%02x", v, v, v))}))
    sca1 = TclTkWidget.new(ip, t1, scale,
      "-label scale -orient h -from 0 -to 255 -variable", v1, "-command", c)
    pack.e(b1, sca1)
  end

  # entry のサンプル.
  def test_entry(ip, parent)
    button, entry, global, pack = ip.commands().values_at(
      "button", "entry", "global", "pack")
    t1, b1, cb = inittoplevel(ip, parent, "entry")

    ## entry

    v1 = TclTkVariable.new(ip, nil)
    global.e(v1)
    # ヌルストリングは "{}" で指定する.
    v1.set("{}")
    en1 = TclTkWidget.new(ip, t1, entry, "-textvariable", v1)
    cb.push(c = TclTkCallback.new(ip, proc{
      global.e(v1); print(v1.get(), "\n"); v1.set("{}")}))
    b2 = TclTkWidget.new(ip, t1, button, "-text print -command", c)
    pack.e(b1, en1, b2)
  end

  # text のサンプル.
  def test_text(ip, parent)
    button, pack, text = ip.commands().values_at(
      "button", "pack", "text")
    t1, b1, cb = inittoplevel(ip, parent, "text")

    ## text

    te1 = TclTkWidget.new(ip, t1, text)
    cb.push(c = TclTkCallback.new(ip, proc{
      # 1 行目の 0 文字目から最後までを表示し, 削除する.
      print(te1.e("get 1.0 end")); te1.e("delete 1.0 end")}))
    b2 = TclTkWidget.new(ip, t1, button, "-text print -command", c)
    pack.e(b1, te1, b2)
  end

  # raise/lower のサンプル.
  def test_raise(ip, parent)
    button, frame, lower, pack, raise = ip.commands().values_at(
      "button", "frame", "lower", "pack", "raise")
    t1, b1, cb = inittoplevel(ip, parent, "raise/lower")

    ## raise/lower

    # button を隠すテストのために, frame を使う.
    f1 = TclTkWidget.new(ip, t1, frame)
    # コールバック内で参照する変数は先に宣言しておかなければならない.
    b2 = nil
    cb.push(c = TclTkCallback.new(ip, proc{raise.e(f1, b2)}))
    b2 = TclTkWidget.new(ip, t1, button, "-text raise -command", c)
    cb.push(c = TclTkCallback.new(ip, proc{lower.e(f1, b2)}))
    b3 = TclTkWidget.new(ip, t1, button, "-text lower -command", c)
    lower.e(f1, b3)

    pack.e(b2, b3, "-in", f1)
    pack.e(b1, f1)
  end

  # modal なウィジェットのサンプル.
  def test_modal(ip, parent)
    button, frame, message, pack, tk_chooseColor, tk_getOpenFile,
      tk_messageBox = ip.commands().values_at(
      "button", "frame", "message", "pack", "tk_chooseColor",
      "tk_getOpenFile", "tk_messageBox")
    # 最初に load されていないライブラリは ip.commands() に存在しないので,
    # TclTkLibCommand を生成する必要がある.
    tk_dialog = TclTkLibCommand.new(ip, "tk_dialog")
    t1, b1, cb = inittoplevel(ip, parent, "message/modal")

    ## message

    mes = "これは message ウィジェットのテストです."
    mes += "以下は modal なウィジェットのテストです."
    me1 = TclTkWidget.new(ip, t1, message, "-text {#{mes}}")

    ## modal

    # tk_messageBox
    cb.push(c = TclTkCallback.new(ip, proc{
      print tk_messageBox.e("-type yesnocancel -message messageBox",
      "-icon error -default cancel -title messageBox"), "\n"}))
    b2 = TclTkWidget.new(ip, t1, button, "-text messageBox -command", c)
    # tk_dialog
    cb.push(c = TclTkCallback.new(ip, proc{
      # ウィジェット名を生成するためにダミーの frame を生成.
      print tk_dialog.e(TclTkWidget.new(ip, t1, frame),
      "dialog dialog error 2 yes no cancel"), "\n"}))
    b3 = TclTkWidget.new(ip, t1, button, "-text dialog -command", c)
    # tk_chooseColor
    cb.push(c = TclTkCallback.new(ip, proc{
      print tk_chooseColor.e("-title chooseColor"), "\n"}))
    b4 = TclTkWidget.new(ip, t1, button, "-text chooseColor -command", c)
    # tk_getOpenFile
    cb.push(c = TclTkCallback.new(ip, proc{
      print tk_getOpenFile.e("-defaultextension .rb",
      "-filetypes {{{Ruby Script} {.rb}} {{All Files} {*}}}",
      "-title getOpenFile"), "\n"}))
    b5 = TclTkWidget.new(ip, t1, button, "-text getOpenFile -command", c)

    pack.e(b1, me1, b2, b3, b4, b5)
  end

  # menu のサンプル.
  def test_menu(ip, parent)
    global, menu, menubutton, pack = ip.commands().values_at(
      "global", "menu", "menubutton", "pack")
    tk_optionMenu = TclTkLibCommand.new(ip, "tk_optionMenu")
    t1, b1, cb = inittoplevel(ip, parent, "menu")

    ## menu

    # menubutton を生成する.
    mb1 = TclTkWidget.new(ip, t1, menubutton, "-text menu")
    # menu を生成する.
    me1 = TclTkWidget.new(ip, mb1, menu)
    # mb1 から me1 が起動されるようにする.
    mb1.e("configure -menu", me1)

    # cascade で起動される menu を生成する.
    me11 = TclTkWidget.new(ip, me1, menu)
    # radiobutton のサンプル.
    v1 = TclTkVariable.new(ip, nil); global.e(v1); v1.set("r1")
    me11.e("add radiobutton -label radio1 -value r1 -variable", v1)
    me11.e("add radiobutton -label radio2 -value r2 -variable", v1)
    me11.e("add radiobutton -label radio3 -value r3 -variable", v1)
    # cascade により mb11 が起動されるようにする.
    me1.e("add cascade -label cascade -menu", me11)

    # checkbutton のサンプル.
    v2 = TclTkVariable.new(ip, nil); global.e(v2); v2.set("none")
    me1.e("add checkbutton -label check -variable", v2)
    # separator のサンプル.
    me1.e("add separator")
    # command のサンプル.
    v3 = nil
    cb.push(c = TclTkCallback.new(ip, proc{
      global.e(v1, v2, v3); print "v1: ", v1.get(), ", v2: ", v2.get(),
      ", v3: ", v3.get(), "\n"}))
    me1.e("add command -label print -command", c)

    ## tk_optionMenu

    v3 = TclTkVariable.new(ip, nil); global.e(v3); v3.set("opt2")
    om1 = TclTkWidget.new(ip, t1, tk_optionMenu, v3, "opt1 opt2 opt3 opt4")

    pack.e(b1, mb1, om1, "-side left")
  end

  # listbox のサンプル.
  def test_listbox(ip, parent)
    clipboard, frame, grid, listbox, lower, menu, menubutton, pack, scrollbar,
      selection = ip.commands().values_at(
      "clipboard", "frame", "grid", "listbox", "lower", "menu", "menubutton",
      "pack", "scrollbar", "selection")
    t1, b1, cb = inittoplevel(ip, parent, "listbox")

    ## listbox/scrollbar

    f1 = TclTkWidget.new(ip, t1, frame)
    # コールバック内で参照する変数は先に宣言しておかなければならない.
    li1 = sc1 = sc2 = nil
    # 実行時に, 後ろにパラメータがつくコールバックは,
    # イテレータ変数でそのパラメータを受け取ることができる.
    # (複数のパラメータはひとつの文字列にまとめられる.)
    cb.push(c1 = TclTkCallback.new(ip, proc{|i| li1.e("xview", i)}))
    cb.push(c2 = TclTkCallback.new(ip, proc{|i| li1.e("yview", i)}))
    cb.push(c3 = TclTkCallback.new(ip, proc{|i| sc1.e("set", i)}))
    cb.push(c4 = TclTkCallback.new(ip, proc{|i| sc2.e("set", i)}))
    # listbox
    li1 = TclTkWidget.new(ip, f1, listbox,
      "-xscrollcommand", c3, "-yscrollcommand", c4,
      "-selectmode extended -exportselection true")
    for i in 1..20
      li1.e("insert end {line #{i} line #{i} line #{i} line #{i} line #{i}}")
    end
    # scrollbar
    sc1 = TclTkWidget.new(ip, f1, scrollbar, "-orient horizontal -command", c1)
    sc2 = TclTkWidget.new(ip, f1, scrollbar, "-orient vertical -command", c2)

    ## selection/clipboard

    mb1 = TclTkWidget.new(ip, t1, menubutton, "-text edit")
    me1 = TclTkWidget.new(ip, mb1, menu)
    mb1.e("configure -menu", me1)
    cb.push(c = TclTkCallback.new(ip, proc{
      # clipboard をクリア.
      clipboard.e("clear")
      # selection から文字列を読み込み clipboard に追加する.
      clipboard.e("append {#{selection.e('get')}}")}))
    me1.e("add command -label {selection -> clipboard} -command",c)
    cb.push(c = TclTkCallback.new(ip, proc{
      # li1 をクリア.
      li1.e("delete 0 end")
      # clipboard から文字列を取り出し, 1 行ずつ
      selection.e("get -selection CLIPBOARD").split(/\n/).each{|line|
        # li1 に挿入する.
        li1.e("insert end {#{line}}")}}))
    me1.e("add command -label {clipboard -> listbox} -command",c)

    grid.e(li1, "-row 0 -column 0 -sticky news")
    grid.e(sc1, "-row 1 -column 0 -sticky ew")
    grid.e(sc2, "-row 0 -column 1 -sticky ns")
    grid.e("rowconfigure", f1, "0 -weight 100")
    grid.e("columnconfigure", f1, "0 -weight 100")
    f2 = TclTkWidget.new(ip, t1, frame)
    lower.e(f2, b1)
    pack.e(b1, mb1, "-in", f2, "-side left")
    pack.e(f2, f1)
  end

  # canvas のサンプル.
  def test_canvas(ip, parent)
    canvas, lower, pack = ip.commands().values_at("canvas", "lower", "pack")
    t1, b1, cb = inittoplevel(ip, parent, "canvas")

    ## canvas

    ca1 = TclTkWidget.new(ip, t1, canvas, "-width 400 -height 300")
    lower.e(ca1, b1)
    # rectangle を作る.
    idr = ca1.e("create rectangle 10 10 20 20")
    # oval を作る.
    ca1.e("create oval 60 10 100 50")
    # polygon を作る.
    ca1.e("create polygon 110 10 110 30 140 10")
    # line を作る.
    ca1.e("create line 150 10 150 30 190 10")
    # arc を作る.
    ca1.e("create arc 200 10 250 50 -start 0 -extent 90 -style pieslice")
    # i1 は本当は, どこかで破壊しなければならないが, 面倒なので放ってある.
    i1 = TclTkImage.new(ip, "photo", "-file maru.gif")
    # image を作る.
    ca1.e("create image 100 100 -image", i1)
    # bitmap を作る.
    ca1.e("create bitmap 260 50 -bitmap questhead")
    # text を作る.
    ca1.e("create text 320 50 -text {drag rectangle}")
    # window を作る(クローズボタン).
    ca1.e("create window 200 200 -window", b1)

    # bind により rectangle を drag できるようにする.
    cb.push(c = TclTkCallback.new(ip, proc{|i|
      # i に x と y を受け取るので, 取り出す.
      x, y = i.split(/ /); x = x.to_f; y = y.to_f
      # 座標を変更する.
      ca1.e("coords current #{x - 5} #{y - 5} #{x + 5} #{y + 5}")},
      # x, y 座標を空白で区切ったものをイテレータ変数へ渡すように指定.
      "%x %y"))
    # rectangle に bind する.
    ca1.e("bind", idr, "<B1-Motion>", c)

    pack.e(ca1)
  end
end

# test driver

if ARGV.size == 0
  print "#{$0} n で, n 個のインタプリタを起動します.\n"
  n = 1
else
  n = ARGV[0].to_i
end

print "start\n"
ip = []

# インタプリタ, ウィジェット等の生成.
for i in 1 .. n
  ip.push(Test1.new())
end

# 用意ができたらイベントループに入る.
TclTk.mainloop()
print "exit from mainloop\n"

# インタプリタが GC されるかのテスト.
ip = []
print "GC.start\n" if $DEBUG
GC.start() if $DEBUG
print "end\n"

exit

# end
