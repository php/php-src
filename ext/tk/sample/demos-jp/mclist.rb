# -*- coding: utf-8 -*-
#
# mclist.rb --
#
# This demonstration script creates a toplevel window containing a Ttk
# tree widget configured as a multi-column listbox.
#
# based on "Id: mclist.tcl,v 1.3 2007/12/13 15:27:07 dgp Exp"

if defined?($mclist_demo) && $mclist_demo
  $mclist_demo.destroy
  $mclist_demo = nil
end

$mclist_demo = TkToplevel.new {|w|
  title("Multi-Column List")
  iconname("mclist")
  positionWindow(w)
}

base_frame = TkFrame.new($mclist_demo).pack(:fill=>:both, :expand=>true)

## Explanatory text
Ttk::Label.new(base_frame, :font=>$font, :wraplength=>'4i',
               :justify=>:left, :anchor=>'n', :padding=>[10, 2, 10, 6],
               :text=><<EOL).pack(:fill=>:x)
Ttkとは，テーマ指定可能な新しいウィジェット集合です．\
Ttk::Treeviewウィジェットは\
Ttkウィジェットセットに含まれるウィジェットの一つで，\
それが保持する木構造のデータそのものまでは表示することなく，\
示したい情報をマルチカラムで表示させることができます．
このサンプルは，複数のカラムを持ったリストボックスを作成する簡単な例です．
各カラムのタイトル(heading)をクリックすれば，\
そのカラムの情報に基づいてリストの並べ替えがなされるはずです．\
また，カラムのタイトル間の区切り部分をドラッグすることで，\
カラムの幅を変更することも可能です．
EOL

## See Code / Dismiss
Ttk::Frame.new(base_frame) {|frame|
  sep = Ttk::Separator.new(frame)
  Tk.grid(sep, :columnspan=>4, :row=>0, :sticky=>'ew', :pady=>2)
  TkGrid('x',
         Ttk::Button.new(frame, :text=>'コード参照',
                         :image=>$image['view'], :compound=>:left,
                         :command=>proc{showCode 'mclist'}),
         Ttk::Button.new(frame, :text=>'閉じる',
                         :image=>$image['delete'], :compound=>:left,
                         :command=>proc{
                           $mclist_demo.destroy
                           $mclist_demo = nil
                         }),
         :padx=>4, :pady=>4)
  grid_columnconfigure(0, :weight=>1)
  pack(:side=>:bottom, :fill=>:x)
}

container = Ttk::Frame.new(base_frame)
tree = Ttk::Treeview.new(base_frame, :columns=>%w(country capital currency),
                          :show=>:headings)
if Tk.windowingsystem != 'aquq'
  vsb = tree.yscrollbar(Ttk::Scrollbar.new(base_frame))
  hsb = tree.xscrollbar(Ttk::Scrollbar.new(base_frame))
else
  vsb = tree.yscrollbar(Tk::Scrollbar.new(base_frame))
  hsb = tree.xscrollbar(Tk::Scrollbar.new(base_frame))
end

container.pack(:fill=>:both, :expand=>true)
Tk.grid(tree, vsb, :in=>container, :sticky=>'nsew')
Tk.grid(hsb,       :in=>container, :sticky=>'nsew')
container.grid_columnconfigure(0, :weight=>1)
container.grid_rowconfigure(0, :weight=>1)

## The data we're going to insert
data = [
  ['アルゼンチン', 	'ブエノスアイレス', 	'ARS'],
  ['オーストラリア',	'キャンベラ',		'AUD'],
  ['ブラジル', 		'ブラジリア', 		'BRL'],
  ['カナダ', 		'オタワ', 		'CAD'],
  ['中国',		'北京', 		'CNY'],
  ['フランス',		'パリ', 		'EUR'],
  ['ドイツ', 		'ベルリン',		'EUR'],
  ['インド', 		'ニューデリー',		'INR'],
  ['イタリア', 		'ローマ', 		'EUR'],
  ['日本', 		'東京', 		'JPY'],
  ['メキシコ', 		'メキシコシティ', 	'MXN'],
  ['ロシア', 		'モスクワ', 		'RUB'],
  ['南アフリカ',	'プレトリア', 		'ZAR'],
  ['英国', 		'ロンドン', 		'GBP'],
  ['アメリカ', 		'ワシントン D.C.', 	'USD'],
]

## Code to insert the data nicely
font = Ttk::Style.lookup(tree[:style], :font)
cols = %w(country capital currency)
cols.zip(%w(国名 首都 通貨)).each{|col, name|
  tree.heading_configure(col, :text=>name,
                         :command=>proc{sort_by(tree, col, false)})
  tree.column_configure(col, :width=>TkFont.measure(font, name))
}

data.each{|country, capital, currency|
  #tree.insert('', :end, :values=>[country, capital, currency])
  tree.insert(nil, :end, :values=>[country, capital, currency])
  cols.zip([country, capital, currency]).each{|col, val|
    len = TkFont.measure(font, "#{val}  ")
    if tree.column_cget(col, :width) < len
      tree.column_configure(col, :width=>len)
    end
  }
}

## Code to do the sorting of the tree contents when clicked on
def sort_by(tree, col, direction)
  tree.children(nil).map!{|row| [tree.get(row, col), row.id]} .
    sort(&((direction)? proc{|x, y| y <=> x}: proc{|x, y| x <=> y})) .
    each_with_index{|info, idx| tree.move(info[1], nil, idx)}

  tree.heading_configure(col, :command=>proc{sort_by(tree, col, ! direction)})
end
