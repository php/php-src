# -*- coding: utf-8 -*-
#
# checkbutton widget demo2 (called by 'widget')
#

# toplevel widget が存在すれば削除する
if defined?($check2_demo) && $check2_demo
  $check2_demo.destroy
  $check2_demo = nil
end

# demo 用の toplevel widget を生成
$check2_demo = TkToplevel.new {|w|
  title("Checkbutton Demonstration 2")
  iconname("check2")
  positionWindow(w)
}

base_frame = TkFrame.new($check2_demo).pack(:fill=>:both, :expand=>true)

# label 生成
msg = TkLabel.new(base_frame) {
  font $font
  wraplength '4i'
  justify 'left'
  text "下には４つのチェックボタンが表示されています。クリックするとボタンの選択状態が変わり、Tcl変数（TkVariableオブジェクトでアクセスできます）にそのボタンの状態を示す値を設定します。最初のボタンの状態は他の３つのボタンの状態にも依存して変化します。もし３つのボタンの一部だけにチェックが付けられている場合、最初のボタンはトライステート（３状態）モードでの表示を行います。現在の変数の値を見るには「変数参照」ボタンをクリックしてください。"
}
msg.pack('side'=>'top')

# 変数生成
safety = TkVariable.new(0)
wipers = TkVariable.new(0)
brakes = TkVariable.new(0)
sober  = TkVariable.new(0)

# frame 生成
TkFrame.new(base_frame) {|frame|
  TkGrid(TkFrame.new(frame, :height=>2, :relief=>:sunken, :bd=>2),
         :columnspan=>4, :row=>0, :sticky=>'ew', :pady=>2)
  TkGrid('x',
         TkButton.new(frame, :text=>'変数参照',
                      :image=>$image['view'], :compound=>:left,
                      :command=>proc{
                        showVars($check2_demo,
                                 ['safety', safety], ['wipers', wipers],
                                 ['brakes', brakes], ['sober', sober])
                      }),
         TkButton.new(frame, :text=>'コード参照',
                      :image=>$image['view'], :compound=>:left,
                      :command=>proc{showCode 'check2'}),
         TkButton.new(frame, :text=>'閉じる',
                      :image=>$image['delete'], :compound=>:left,
                      :command=>proc{
                        tmppath = $check2_demo
                        $check2_demo = nil
                        $showVarsWin[tmppath.path] = nil
                        tmppath.destroy
                      }),
         :padx=>4, :pady=>4)
  frame.grid_columnconfigure(0, :weight=>1)
}.pack('side'=>'bottom', 'fill'=>'x')


# checkbutton 生成
TkCheckButton.new(base_frame, :text=>'安全性検査', :variable=>safety,
                  :relief=>:flat, :onvalue=>'all', :offvalue=>'none',
                  :tristatevalue=>'partial'){
  pack('side'=>'top', 'pady'=>2, 'anchor'=>'w')
}

[ TkCheckButton.new(base_frame, 'text'=>'ワイパー OK', 'variable'=>wipers),
  TkCheckButton.new(base_frame, 'text'=>'ブレーキ OK', 'variable'=>brakes),
  TkCheckButton.new(base_frame, 'text'=>'運転手 素面', 'variable'=>sober)
].each{|w|
  w.relief('flat')
  w.pack('side'=>'top', 'padx'=>15, 'pady'=>2, 'anchor'=>'w')
}

# tristate check
in_check = false
tristate_check = proc{|n1,n2,op|
  unless in_check
    in_check = true
    begin
      if n1 == safety
        if safety == 'none'
          wipers.value = 0
          brakes.value = 0
          sober.value  = 0
        elsif safety == 'all'
          wipers.value = 1
          brakes.value = 1
          sober.value  = 1
        end
      else
        if wipers == 1 && brakes == 1 && sober == 1
          safety.value = 'all'
        elsif wipers == 1 || brakes == 1 || sober == 1
          safety.value = 'partial'
        else
          safety.value = 'none'
        end
      end
    ensure
      in_check = false
    end
  end
}

[wipers, brakes, sober, safety].each{|v| v.trace('w', tristate_check)}
