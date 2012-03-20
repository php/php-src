#
#  dnd demo  ---  called from demo.rb
#
unless Object.const_defined?('DemoVar')
  fail RuntimeError, "This is NOT a stand alone script. This script is called from 'demo.rb'. "
end

module DemoDnD
  def self.create(nb)
    frame = nb.insert('end', 'demoDnD', :text=>'Drag and Drop')

    titf1 = Tk::BWidget::TitleFrame.new(frame, :text=>'Drag source')
    subf = titf1.get_frame

    ent1 = Tk::BWidget::LabelEntry.new(subf, :label=>'Entry',
                                       :labelwidth=>14, :dragenabled=>true,
                                       :dragevent=>3)
    labf1 = Tk::BWidget::LabelFrame.new(subf, :text=>'Label (text)',
                                        :width=>14)
    f = labf1.get_frame
    lab = Tk::BWidget::Label.new(f, :text=>'Drag this text',
                                 :dragenabled=>true, :dragevent=>3).pack

    labf2 = Tk::BWidget::LabelFrame.new(subf, :text=>'Label (bitmap)',
                                        :width=>14)
    f = labf2.get_frame
    lab = Tk::BWidget::Label.new(f, :bitmap=>'info',
                                 :dragenabled=>true, :dragevent=>3).pack

    Tk.pack(ent1, labf1, labf2, :side=>:top, :fill=>:x, :pady=>4)

    titf2 = Tk::BWidget::TitleFrame.new(frame, :text=>'Drop targets')
    subf = titf2.get_frame

    ent1 = Tk::BWidget::LabelEntry.new(subf, :label=>'Entry',
                                       :labelwidth=>14, :dropenabled=>true)
    labf1 = Tk::BWidget::LabelFrame.new(subf, :text=>'Label', :width=>14)
    f = labf1.get_frame
    lab = Tk::BWidget::Label.new(f, :dropenabled=>true,
                                 :highlightthickness=>1).pack(:fill=>:x)
    Tk.pack(ent1, labf1, :side=>:top, :fill=>:x, :pady=>4)
    Tk.pack(titf1, titf2, :pady=>4)

    frame
  end
end
