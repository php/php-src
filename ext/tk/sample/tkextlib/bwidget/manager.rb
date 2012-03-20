#
#  manager demo  ---  called from demo.rb
#
unless Object.const_defined?('DemoVar')
  fail RuntimeError, "This is NOT a stand alone script. This script is called from 'demo.rb'. "
end

module DemoManager
  @@progress    = TkVariable.new(false)
  @@status      = TkVariable.new('Compute in progress...')
  @@homogeneous = TkVariable.new(false)
  @@constw      = TkVariable.new
  @@afterobj    = nil

  def self.create(nb)
    frame = nb.insert('end', 'demoManager', :text=>'Manager')

    topf = TkFrame.new(frame)
    titf1 = Tk::BWidget::TitleFrame.new(topf, :text=>"MainFrame")
    titf2 = Tk::BWidget::TitleFrame.new(topf, :text=>"NoteBook")
    titf3 = Tk::BWidget::TitleFrame.new(frame, :text=>"Paned & ScrolledWindow")

    _mainframe(titf1.get_frame)
    _notebook(titf2.get_frame)
    _paned(titf3.get_frame)

    Tk.pack(titf1, titf2, :padx=>4, :side=>:left, :fill=>:both, :expand=>true)
    Tk.pack(topf, :fill=>:x, :pady=>2)
    Tk.pack(titf3, :pady=>2, :padx=>4, :fill=>:both, :expand=>true)

    frame
  end

  def self._mainframe(parent)
    labf1 = Tk::BWidget::LabelFrame.new(parent, :text=>'Toolbar',
                                        :side=>:top, :anchor=>:w,
                                        :relief=>:sunken, :borderwidth=>2)
    subf = labf1.get_frame
    chk1 = TkCheckbutton.new(subf, :text=>'View toolbar 1',
                             :variable=>DemoVar.toolbar1,
                             :command=>proc{
                               DemoVar.mainframe.show_toolbar(
                                  0, DemoVar.toolbar1.value
                               )
                             })
    chk2 = TkCheckbutton.new(subf, :text=>'View toolbar 2',
                             :variable=>DemoVar.toolbar2,
                             :command=>proc{
                               DemoVar.mainframe.show_toolbar(
                                  1, DemoVar.toolbar2.value
                               )
                             })

    Tk.pack(chk1, chk2, :anchor=>:w, :fill=>:x)
    labf1.pack(:fill=>:both)

    labf2 = Tk::BWidget::LabelFrame.new(parent, :text=>'Status bar',
                                        :side=>:top, :anchor=>:w,
                                        :relief=>:sunken, :borderwidth=>2)
    subf = labf2.get_frame
    chk1 = TkCheckbutton.new(subf, :text=>"Show Progress\nindicator",
                             :justify=>:left, :variable=>@@progress,
                             :command=>proc{ _show_progress })
    chk1.pack(:anchor=>:w, :fill=>:x)

    Tk.pack(labf1, labf2, :side=>:left, :padx=>4, :fill=>:both)
  end

  def self._notebook(parent)
    TkCheckbutton.new(parent, :text=>'Homogeneous label',
                      :variable=>@@homogeneous,
                      :command=>proc{
                        DemoVar.notebook[:homogeneous] = @@homogeneous.value
                      }).pack(:side=>:left, :anchor=>:n, :fill=>:x)
  end

  def self._paned(parent)
    pw1   = Tk::BWidget::PanedWindow.new(parent, :side=>:top)
    pane  = pw1.add(:minsize=>100)

    pw2   = Tk::BWidget::PanedWindow.new(pane, :side=>:left)
    pane1 = pw2.add(:minsize=>100)
    pane2 = pw2.add(:minsize=>100)

    pane3 = pw1.add(:minsize=>100)

    [pane1, pane2].each{|pane|
      sw = Tk::BWidget::ScrolledWindow.new(pane)
      lb = TkListbox.new(sw, :height=>8, :width=>20, :highlightthickness=>0)
      (1..8).each{|i| lb.insert('end', "Valur #{i}") }
      sw.set_widget(lb)
      sw.pack(:fill=>:both, :expand=>true)
    }

    sw = Tk::BWidget::ScrolledWindow.new(pane3, :relief=>:sunken,
                                         :borderwidth=>2)
    sf = Tk::BWidget::ScrollableFrame.new(sw)
    sw.set_widget(sf)
    subf = sf.get_frame
    lab = TkLabel.new(subf, :text=>'This is a ScrollableFrame')
    chk = TkCheckbutton.new(subf, :text=>'Constrained with',
                            :variable=>@@constw, :command=>proc{
                              sf['constrainedwidth'] = @@constw.value
                            })
    lab.pack
    chk.pack(:anchor=>:w)
    chk.bind('FocusIn', proc{sf.see(chk)})
    (0..20).each{|i|
      ent = TkEntry.new(subf, :width=>50).pack(:fill=>:x, :pady=>4)
      ent.bind('FocusIn', proc{sf.see(ent)})
      ent.insert('end', "Text field #{i}")
    }

    Tk.pack(sw, pw2, pw1, :fill=>:both, :expand=>true)
  end

  def self._show_progress
    unless @@afterobj
      @@afterobj = TkTimer.new(30, -1, proc{_update_progress})
    end
    if @@progress.bool
      DemoVar.status.value = 'Compute in progress...'
      DemoVar.prgindic.value = 0
      DemoVar.mainframe.show_statusbar(:progression)
      @@afterobj.start unless @@afterobj.running?
    else
      DemoVar.status.value = ''
      DemoVar.mainframe.show_statusbar(:status)
      @@afterobj.stop
    end
  end

  def self._update_progress
    if @@progress.bool
      if DemoVar.prgindic.numeric < 100
        DemoVar.prgindic.numeric += 5
      else
        @@progress.value = false
        DemoVar.mainframe.show_statusbar(:status)
        DemoVar.status.value = 'Done'
        @@afterobj.stop
        Tk.after(500, proc{ DemoVar.status.value = '' })
      end
    else
      @@afterobj.stop
    end
  end

end

