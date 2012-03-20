#
#  basic demo  ---  called from demo.rb
#
unless Object.const_defined?('DemoVar')
  fail RuntimeError, "This is NOT a stand alone script. This script is called from 'demo.rb'. "
end

module DemoBasic
  @@var   = TkVariable.new_hash
  @@after = nil
  @@count = 0

  def self.create(nb)
    frame = nb.insert('end', 'demoBasic', :text=>'Basic')
    topf  = TkFrame.new(frame)

    titf1 = Tk::BWidget::TitleFrame.new(topf, :text=>'Label')
    titf2 = Tk::BWidget::TitleFrame.new(topf, :text=>'Entry')

    titf3 = Tk::BWidget::TitleFrame.new(frame, :text=>'Button and ArrowButton')

    _label(titf1.get_frame)
    _entry(titf2.get_frame)
    _button(titf3.get_frame)

    Tk.pack(titf1, titf2, :side=>:left, :fill=>:both, :padx=>4, :expand=>true)
    topf.pack(:pady=>2, :fill=>:x)
    titf3.pack(:pady=>2, :padx=>4, :fill=>:x)
  end

  def self._label(parent)
    lab = Tk::BWidget::Label.new(parent, :text=>'This is a Label widget',
                                 :helptext=>'Label widget')
    chk = TkCheckbutton.new(parent, :text=>'Disabled',
                            :variable=>@@var.ref(lab, 'state'),
                            :onvalue=>'disabled', :offvalue=>'normal',
                            :command=>proc{lab[:state] = @@var[lab, 'state']})
    lab.pack(:anchor=>:w, :pady=>4)
    chk.pack(:anchor=>:w)
  end

  def self._entry(parent)
    ent = Tk::BWidget::Entry.new(parent, :text=>'Press enter',
                                 :helptext=>'Entry widtet',
                                 :command=>proc{
                                   @@var['entcmd'] = 'command called'
                                   Tk.after(500, proc{@@var['entcmd'] = ''})
                                 })

    chk1 = TkCheckbutton.new(parent, :text=>'Disabled',
                             :variable=>@@var.ref(ent, 'state'),
                             :onvalue=>'disabled', :offvalue=>'normal',
                             :command=>proc{ent.state = @@var[ent, 'state']})

    chk2 = TkCheckbutton.new(parent, :text=>'Non editable',
                             :variable=>@@var.ref(ent, 'editable'),
                             :onvalue=>false, :offvalue=>true,
                             :command=>proc{
                               ent.editable = @@var[ent, 'editable']
                             })

    lab = TkLabel.new(parent, :textvariable=>@@var.ref('entcmd'),
                      :foreground=>'red')

    ent.pack(:pady=>4, :anchor=>:w)
    Tk.pack(chk1, chk2, :anchor=>:w)
    lab.pack(:pady=>4)
  end

  def self._button(parent)
    frame = TkFrame.new(parent)
    but  = Tk::BWidget::Button.new(frame, :text=>'Press me!',
                                   :repeatdelay=>300,
                                   :command=>proc{_butcmd('command')},
                                   :helptext=>'This is a Button widget')

    sep1 = Tk::BWidget::Separator.new(frame, :orient=>:vertical)
    arr1 = Tk::BWidget::ArrowButton.new(frame, :type=>:button,
                :width=>25, :height=>25, :repeatdelay=>300,
                :command=>proc{_butcmd('command')},
                :helptext=>"This is an ArrowButton widget\nof type button")

    sep2 = Tk::BWidget::Separator.new(frame, :orient=>:vertical)
    arr2 = Tk::BWidget::ArrowButton.new(frame, :type=>:arrow,
                :width=>25, :height=>25, :relief=>:sunken,
                :ipadx=>0, :ipady=>0, :repeatdelay=>300,
                :command=>proc{_butcmd('command')},
                :helptext=>"This is an ArrowButton widget\nof type arrow")

    but.pack(:side=>:left, :padx=>4)
    sep1.pack(:side=>:left, :padx=>4, :fill=>:y)
    arr1.pack(:side=>:left, :padx=>4)
    sep2.pack(:side=>:left, :padx=>4, :fill=>:y)
    arr2.pack(:side=>:left, :padx=>4)
    frame.pack

    Tk::BWidget::Separator.new(parent,
                               :orient=>:horizontal).pack(:fill=>:x, :pady=>10)

    labf1 = Tk::BWidget::LabelFrame.new(parent, :text=>'Command',
                                        :side=>:top, :anchor=>:w,
                                        :relief=>:sunken, :borderwidth=>1)
    subf = labf1.get_frame
    chk1 = TkCheckbutton.new(subf, :text=>'Disabled',
                             :variable=>@@var.ref('bstate'),
                             :onvalue=>'disabled', :offvalue=>'normal',
                             :command=>proc{_bstate(@@var['bstate'],
                                                     but, arr1, arr2)})
    chk2 = TkCheckbutton.new(subf, :text=>"Use armcommand/\ndisarmcommand",
                             :variable=>@@var.ref('barmcmd'),
                             :command=>proc{_barmcmd(@@var['barmcmd'],
                                                     but, arr1, arr2)})
    Tk.pack(chk1, chk2, :anchor=>:w)

    label = TkLabel.new(parent, :textvariable=>@@var.ref('butcmd'),
                        :foreground=>'red').pack(:side=>:bottom, :pady=>4)

    labf2 = Tk::BWidget::LabelFrame.new(parent, :text=>'Direction',
                                        :side=>:top, :anchor=>:w,
                                        :relief=>:sunken, :borderwidth=>1)
    subf = labf2.get_frame
    @@var['bside'] = :top
    [:top, :left, :bottom, :right].each{|dir|
      TkRadiobutton.new(subf, :text=>"#{dir} arrow",
                        :variable=>@@var.ref('bside'), :value=>dir,
                        :command=>proc{_bside(@@var['bside'], arr1, arr2)}
                        ).pack(:anchor=>:w)
    }

    labf3 = Tk::BWidget::LabelFrame.new(parent, :text=>'Relief',
                                        :side=>:top, :anchor=>:w,
                                        :relief=>:sunken, :borderwidth=>1)
    subf = labf3.get_frame
    @@var['brelief'] = :raised
    [ %w(raised sunken ridge groove), %w(flat solid link)].each{|lrelief|
      f = TkFrame.new(subf)
      lrelief.each{|relief|
        TkRadiobutton.new(f, :text=>relief,
                          :variable=>@@var.ref('brelief'), :value=>relief,
                          :command=>proc{
                            _brelief(@@var['brelief'], but, arr1, arr2)
                          }).pack(:anchor=>:w)
      }
      f.pack(:side=>:left, :padx=>2, :anchor=>:n)
    }
    Tk.pack(labf1, labf2, labf3, :side=>:left, :fill=>:y, :padx=>4)
  end

  def self._bstate(state, but, arr1, arr2)
    [but, arr1, arr2].each{|b| b[:state] = state}
  end

  def self._brelief(relief, but, arr1, arr2)
    but[:relief] = relief
    if relief.to_s != 'link'
      [arr1, arr2].each{|arr| arr[:relief] = relief}
    end
  end

  def self._bside(side, *args)
    args.each{|arr| arr[:dir] = side}
  end

  def self._barmcmd(value, but, arr1, arr2)
    if TkComm.bool(value)
      but.configure(:armcommand=>proc{_butcmd('arm')},
                    :disarmcommand=>proc{_butcmd('disarm')},
                    :command=>'')
      [arr1, arr2].each{|arr|
        arr.configure(:armcommand=>proc{_butcmd('arm')},
                      :disarmcommand=>proc{_butcmd('disarm')},
                      :command=>'')
      }
    else
      but.configure(:armcommand=>'', :disarmcommand=>'',
                    :command=>proc{_butcmd('command')})
      [arr1, arr2].each{|arr|
        arr.configure(:armcommand=>'', :disarmcommand=>'',
                      :command=>proc{_butcmd('command')})
      }
    end
  end

  def self._butcmd(reason)
    unless @@after
      @@after = TkTimer.new(500, 1, proc{@@var['butcmd'] = ''})
    end
    @@after.stop
    if (reason == 'arm')
      @@count += 1
      @@var['butcmd'] = "#{reason} command called (#{@@count})"
    else
      @@count = 0
      @@var['butcmd'] = "#{reason} command called"
    end
    @@after.start
  end
end
