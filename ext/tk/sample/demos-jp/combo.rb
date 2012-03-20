# -*- coding: utf-8 -*-
#
# combo.rb --
#
# This demonstration script creates several combobox widgets.
#
# based on "Id: combo.tcl,v 1.3 2007/12/13 15:27:07 dgp Exp"

if defined?($combo_demo) && $combo_demo
  $combo_demo.destroy
  $combo_demo = nil
end

$combo_demo = TkToplevel.new {|w|
  title("Combobox Demonstration")
  iconname("combo")
  positionWindow(w)
}

base_frame = TkFrame.new($combo_demo).pack(:fill=>:both, :expand=>true)

Ttk::Label.new(base_frame, :font=>$font, :wraplength=>'5i', :justify=>:left,
               :text=><<EOL).pack(:side=>:top, :fill=>:x)
以下では3種類のコンボボックスが表示されています．\
最初のものは，エントリウィジェットと同じ様に，\
ポイントしたり，クリックしたり，タイプしたりすることができます．\
また，Returnキーを入力すれば現在の値がリストに追加され，\
ドロップダウンリストから選択することができるようになります．\
↓(下向き矢印)キーを押して表示されたリストから\
矢印キーで他の候補を選んでReturnキーを押せば，値を選択できます．\
2番目のコンボボックスは特定の値に固定されており，一切変更できません．\
3番目のものはオーストラリアの都市のドロップダウンリストから\
選択することだけが可能となっています．
EOL

## variables
firstValue  = TkVariable.new
secondValue = TkVariable.new
ozCity      = TkVariable.new

## See Code / Dismiss buttons
Ttk::Frame.new(base_frame) {|frame|
  sep = Ttk::Separator.new(frame)
  Tk.grid(sep, :columnspan=>4, :row=>0, :sticky=>'ew', :pady=>2)
  TkGrid('x',
         Ttk::Button.new(frame, :text=>'変数参照',
                         :image=>$image['view'], :compound=>:left,
                         :command=>proc{
                           showVars(base_frame,
                                    ['firstVariable', firstValue],
                                    ['secondVariable', secondValue],
                                    ['ozCity', ozCity])
                         }),
         Ttk::Button.new(frame, :text=>'コード参照',
                         :image=>$image['view'], :compound=>:left,
                         :command=>proc{showCode 'combo'}),
         Ttk::Button.new(frame, :text=>'閉じる',
                         :image=>$image['delete'], :compound=>:left,
                         :command=>proc{
                           $combo_demo.destroy
                           $combo_demo = nil
                         }),
         :padx=>4, :pady=>4)
  grid_columnconfigure(0, :weight=>1)
  pack(:side=>:bottom, :fill=>:x)
}

frame = Ttk::Frame.new(base_frame).pack(:fill=>:both, :expand=>true)

australianCities = [
  'キャンベラ', 'シドニー', 'メルボルン', 'パース', 'アデレード',
  'ブリスベーン', 'ホバート', 'ダーウィン', 'アリス スプリングス'
]


secondValue.value = '変更不可'
ozCity.value = 'シドニー'

Tk.pack(Ttk::Labelframe.new(frame, :text=>'Fully Editable'){|f|
          Ttk::Combobox.new(f, :textvariable=>firstValue){|b|
            b.bind('Return', '%W'){|w|
              w.values <<= w.value unless w.values.include?(w.value)
            }
          }.pack(:pady=>5, :padx=>10)
        },

        Ttk::LabelFrame.new(frame, :text=>'Disabled'){|f|
          Ttk::Combobox.new(f, :textvariable=>secondValue, :state=>:disabled) .
            pack(:pady=>5, :padx=>10)
        },

        Ttk::LabelFrame.new(frame, :text=>'Defined List Only'){|f|
          Ttk::Combobox.new(f, :textvariable=>ozCity, :state=>:readonly,
                            :values=>australianCities) .
            pack(:pady=>5, :padx=>10)
        },

        :side=>:top, :pady=>5, :padx=>10)
