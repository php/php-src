#
#  select demo  ---  called from demo.rb
#
unless Object.const_defined?('DemoVar')
  fail RuntimeError, "This is NOT a stand alone script. This script is called from 'demo.rb'. "
end

module DemoSelect
  @@var = TkVariable.new_hash

  def self.create(nb)
    frame = nb.insert('end', 'demoSelect', :text=>'Spin & Combo')

    titf1 = Tk::BWidget::TitleFrame.new(frame, :text=>'SpinBox')
    subf = titf1.get_frame
    spin = Tk::BWidget::SpinBox.new(subf, :range=>[1, 100, 1],
                                    :textvariable=>@@var.ref('spin', 'var'),
                                    :helptext=>'This is the SpinBox')
    ent = Tk::BWidget::LabelEntry.new(subf, :label=>'Linked var',
                                      :labelwidth=>10, :labelanchor=>:w,
                                      :textvariable=>@@var.ref('spin', 'var'),
                                      :editable=>0,
                                      :helptext=>"This is an Entry reflecting\nthe linked var of SpinBox")
    labf = Tk::BWidget::LabelFrame.new(subf, :text=>'Options',
                                       :side=>:top, :anchor=>:w,
                                       :relief=>:sunken, :borderwidth=>1,
                                       :helptext=>'Modify some options of SpinBox')
    subf = labf.get_frame
    chk1 = TkCheckbutton.new(subf, :text=>'Non editable',
                             :variable=>@@var.ref('spin', 'editable'),
                             :onvalue=>false, :offvalue=>true,
                             :command=>proc{
                               spin.editable(@@var['spin', 'editable'])
                             })
    chk2 = TkCheckbutton.new(subf, :text=>'Disabled',
                             :variable=>@@var.ref('spin', 'state'),
                             :onvalue=>'disabled', :offvalue=>'normal',
                             :command=>proc{
                               spin.state(@@var['spin', 'state'])
                             })
    Tk.pack(chk1, chk2, :side=>:left, :anchor=>:w)
    Tk.pack(spin, ent, labf, :pady=>4, :fill=>:x)
    titf1.pack

    titf2 = Tk::BWidget::TitleFrame.new(frame, :text=>'ComboBox')
    subf = titf2.get_frame
    combo = Tk::BWidget::ComboBox.new(subf,
                                      :textvariable=>@@var.ref('combo', 'var'),
                                      :values=>[
                                        'first value', 'second value',
                                        'third value', 'fourth value',
                                        'fifth value'
                                      ],
                                      :helptext=>'This is the ComboBox')
    ent = Tk::BWidget::LabelEntry.new(subf, :label=>'Linked var',
                                      :labelwidth=>10, :labelanchor=>:w,
                                      :textvariable=>@@var.ref('combo', 'var'),
                                      :editable=>0, :helptext=>"This is an Entry reflecting\nthe linked var of ComboBox")
    labf = Tk::BWidget::LabelFrame.new(subf, :text=>'Options', :side=>:top,
                                       :anchor=>:w, :relief=>:sunken,
                                       :borderwidth=>1, :helptext=>'Modify some options of ComboBox')
    subf = labf.get_frame
    chk1 = TkCheckbutton.new(subf, :text=>'Non editable',
                             :variable=>@@var.ref('combo', 'editable'),
                             :onvalue=>false, :offvalue=>true,
                             :command=>proc{
                               combo.editable(@@var['combo', 'editable'])
                             })
    chk2 = TkCheckbutton.new(subf, :text=>'Disabled',
                             :variable=>@@var.ref('combo', 'state'),
                             :onvalue=>'disabled', :offvalue=>'normal',
                             :command=>proc{
                               combo.state(@@var['combo', 'state'])
                             })

    Tk.pack(chk1, chk2, :side=>:left, :anchor=>:w)
    Tk.pack(combo, ent, labf, :pady=>4, :fill=>:x)
    Tk.pack(titf1, titf2, :pady=>4)

    frame
  end
end
