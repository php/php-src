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
Three different combo-boxes are displayed below. \
You can add characters to the first \
one by pointing, clicking and typing, just as with an entry; pressing \
Return will cause the current value to be added to the list that is \
selectable from the drop-down list, and you can choose other values \
by pressing the Down key, using the arrow keys to pick another one, \
and pressing Return again. The second combo-box is fixed to a \
particular value, and cannot be modified at all. The third one only \
allows you to select values from its drop-down list of Australian \
cities.
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
         Ttk::Button.new(frame, :text=>'See Variables',
                         :image=>$image['view'], :compound=>:left,
                         :command=>proc{
                           showVars(base_frame,
                                    ['firstVariable', firstValue],
                                    ['secondVariable', secondValue],
                                    ['ozCity', ozCity])
                         }),
         Ttk::Button.new(frame, :text=>'See Code',
                         :image=>$image['view'], :compound=>:left,
                         :command=>proc{showCode 'combo'}),
         Ttk::Button.new(frame, :text=>'Dismiss',
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
  'Canberra', 'Sydney', 'Melbourne', 'Perth', 'Adelaide', 'Brisbane',
  'Hobart', 'Darwin', 'Alice Springs'
]


secondValue.value = 'unchangable'
ozCity.value = 'Sydney'

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
