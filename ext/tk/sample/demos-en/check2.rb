#
# checkbutton widget demo2 (called by 'widget')
#

# delete old toplevel widget
if defined?($check2_demo) && $check2_demo
  $check2_demo.destroy
  $check2_demo = nil
end

# create demo toplevel widget
$check2_demo = TkToplevel.new {|w|
  title("Checkbutton Demonstration 2")
  iconname("check2")
  positionWindow(w)
}

base_frame = TkFrame.new($check2_demo).pack(:fill=>:both, :expand=>true)

# label
msg = TkLabel.new(base_frame) {
  font $font
  wraplength '4i'
  justify 'left'
  text "Four checkbuttons are displayed below.  If you click on a button, it will toggle the button's selection state and set a Tcl variable to a value indicating the state of the checkbutton.  The first button also follows the state of the other three.  If only some of the three are checked, the first button will display the tri-state mode. Click the \"See Variables\" button to see the current values of the variables."
}
msg.pack('side'=>'top')

# variable
safety = TkVariable.new(0)
wipers = TkVariable.new(0)
brakes = TkVariable.new(0)
sober  = TkVariable.new(0)

# frame
TkFrame.new(base_frame) {|frame|
  TkGrid(TkFrame.new(frame, :height=>2, :relief=>:sunken, :bd=>2),
         :columnspan=>4, :row=>0, :sticky=>'ew', :pady=>2)
  TkGrid('x',
         TkButton.new(frame, :text=>'See Variables',
                      :image=>$image['view'], :compound=>:left,
                      :command=>proc{
                        showVars($check2_demo,
                                 ['safety', safety], ['wipers', wipers],
                                 ['brakes', brakes], ['sober', sober])
                      }),
         TkButton.new(frame, :text=>'See Code',
                      :image=>$image['view'], :compound=>:left,
                      :command=>proc{showCode 'check2'}),
         TkButton.new(frame, :text=>'Dismiss',
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


# checkbutton
TkCheckButton.new(base_frame, :text=>'Safety Check', :variable=>safety,
                  :relief=>:flat, :onvalue=>'all', :offvalue=>'none',
                  :tristatevalue=>'partial'){
  pack('side'=>'top', 'pady'=>2, 'anchor'=>'w')
}

[ TkCheckButton.new(base_frame, 'text'=>'Wipers OK', 'variable'=>wipers),
  TkCheckButton.new(base_frame, 'text'=>'Brakes OK', 'variable'=>brakes),
  TkCheckButton.new(base_frame, 'text'=>'Driver Sober', 'variable'=>sober)
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
