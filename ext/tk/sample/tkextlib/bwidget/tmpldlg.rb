#
#  templdlg demo  ---  called from demo.rb
#
unless Object.const_defined?('DemoVar')
  fail RuntimeError, "This is NOT a stand alone script. This script is called from 'demo.rb'. "
end

module DemoDialog
  @@tmpl      = TkVariable.new_hash
  @@msg       = TkVariable.new_hash
  @@msgdlg    = nil
  @@progmsg   = TkVariable.new
  @@progval   = TkVariable.new
  @@progdlg   = nil
  @@resources = TkVariable.new('en')

  def self.create(nb)
    frame = nb.insert('end', 'demoDlg', :text=>'Dialog')

    titf1 = Tk::BWidget::TitleFrame.new(frame, :text=>'Resources')
    titf2 = Tk::BWidget::TitleFrame.new(frame, :text=>'Template Dialog')
    titf3 = Tk::BWidget::TitleFrame.new(frame, :text=>'Message Dialog')
    titf4 = Tk::BWidget::TitleFrame.new(frame, :text=>'Other dialog')

    subf = titf1.get_frame
    cmd = proc{ TkOption.read_file(File.join(Tk::BWidget::LIBRARY, 'lang',
                                             @@resources.value + '.rc')) }
    Tk.pack(TkRadiobutton.new(subf, :text=>'English', :value=>'en',
                              :variable=>@@resources, :command=>cmd),
            TkRadiobutton.new(subf, :text=>'French', :value=>'fr',
                              :variable=>@@resources, :command=>cmd),
            TkRadiobutton.new(subf, :text=>'German', :value=>'de',
                              :variable=>@@resources, :command=>cmd),
            :side=>:left)

    _tmpldlg(titf2.get_frame)
    _msgdlg(titf3.get_frame)
    _stddlg(titf4.get_frame)

    titf1.pack(:fill=>:x, :pady=>2, :padx=>2)
    titf4.pack(:side=>:bottom, :fill=>:x, :pady=>2, :padx=>2)
    Tk.pack(titf2, titf3, :side=>:left, :padx=>2, :fill=>:both, :expand=>true)
  end

  def self._tmpldlg(parent)
    @@tmpl['side'] = :bottom
    @@tmpl['anchor'] = :c

    labf1 = Tk::BWidget::LabelFrame.new(parent, :text=>'Button side',
                                        :side=>:top, :anchor=>:w,
                                        :relief=>:sunken, :borderwidth=>1)
    subf = labf1.get_frame
    Tk.pack(TkRadiobutton.new(subf, :text=>'Bottom', :value=>:bottom,
                              :variable=>@@tmpl.ref('side'), :anchor=>:w),
            TkRadiobutton.new(subf, :text=>'Left', :value=>:left,
                              :variable=>@@tmpl.ref('side'), :anchor=>:w),
            TkRadiobutton.new(subf, :text=>'Right', :value=>:right,
                              :variable=>@@tmpl.ref('side'), :anchor=>:w),
            TkRadiobutton.new(subf, :text=>'Top', :value=>:top,
                              :variable=>@@tmpl.ref('side'), :anchor=>:w),
            :fill=>:x, :anchor=>:w)

    labf2 = Tk::BWidget::LabelFrame.new(parent, :text=>'Button anchor',
                                        :side=>:top, :anchor=>:w,
                                        :relief=>:sunken, :borderwidth=>1)
    subf = labf2.get_frame
    Tk.pack(TkRadiobutton.new(subf, :text=>'North', :value=>:n,
                              :variable=>@@tmpl.ref('anchor'), :anchor=>:w),
            TkRadiobutton.new(subf, :text=>'West', :value=>:w,
                              :variable=>@@tmpl.ref('anchor'), :anchor=>:w),
            TkRadiobutton.new(subf, :text=>'East', :value=>:e,
                              :variable=>@@tmpl.ref('anchor'), :anchor=>:w),
            TkRadiobutton.new(subf, :text=>'South', :value=>:s,
                              :variable=>@@tmpl.ref('anchor'), :anchor=>:w),
            TkRadiobutton.new(subf, :text=>'Center', :value=>:c,
                              :variable=>@@tmpl.ref('anchor'), :anchor=>:w),
            :fill=>:x, :anchor=>:w)

    sep = Tk::BWidget::Separator.new(parent, :orient=>:horizontal)
    button = TkButton.new(parent, :text=>'Show', :command=>proc{_show_tmpldlg})

    button.pack(:side=>:bottom)
    sep.pack(:side=>:bottom, :fill=>:x, :pady=>10)
    Tk.pack(labf1, labf2, :side=>:left, :padx=>4, :anchor=>:n)
  end

  def self._msgdlg(parent)
    @@msg['type'] = 'ok'
    @@msg['icon'] = 'info'

    labf1 = Tk::BWidget::LabelFrame.new(parent, :text=>'Type', :side=>:top,
                                        :anchor=>:w, :relief=>:sunken,
                                        :borderwidth=>1)
    subf = labf1.get_frame
    Tk.pack(TkRadiobutton.new(subf, :text=>'Ok', :value=>'ok',
                              :variable=>@@msg.ref('type'), :anchor=>:w),
            TkRadiobutton.new(subf, :text=>'Ok, Cancel', :value=>'okcancel',
                              :variable=>@@msg.ref('type'), :anchor=>:w),
            TkRadiobutton.new(subf, :text=>'Retry, Cancel',
                              :value=>'retrycancel',
                              :variable=>@@msg.ref('type'), :anchor=>:w),
            TkRadiobutton.new(subf, :text=>'Yes, No', :value=>'yesno',
                              :variable=>@@msg.ref('type'), :anchor=>:w),
            TkRadiobutton.new(subf, :text=>'Yes, No, Cancel',
                              :value=>'yesnocancel',
                              :variable=>@@msg.ref('type'), :anchor=>:w),
            TkRadiobutton.new(subf, :text=>'Abort, Retry, Ignore',
                              :value=>'abortretryignore',
                              :variable=>@@msg.ref('type'), :anchor=>:w),
            :fill=>:x, :anchor=>:w)

    Tk.pack(TkRadiobutton.new(subf, :text=>'User', :value=>'user',
                              :variable=>@@msg.ref('type'), :anchor=>:w),
            Tk::BWidget::Entry.new(subf, :textvariable=>@@msg.ref('buttons')),
            :side=>:left)

    labf2 = Tk::BWidget::LabelFrame.new(parent, :text=>'Icon', :side=>:top,
                                        :anchor=>:w, :relief=>:sunken,
                                        :borderwidth=>1)
    subf = labf2.get_frame
    Tk.pack(TkRadiobutton.new(subf, :text=>'Information', :value=>'info',
                              :variable=>@@msg.ref('icon'), :anchor=>:w),
            TkRadiobutton.new(subf, :text=>'Question',    :value=>'question',
                              :variable=>@@msg.ref('icon'), :anchor=>:w),
            TkRadiobutton.new(subf, :text=>'Warning',     :value=>'warning',
                              :variable=>@@msg.ref('icon'), :anchor=>:w),
            TkRadiobutton.new(subf, :text=>'Error',       :value=>'error',
                              :variable=>@@msg.ref('icon'), :anchor=>:w),
            :fill=>:x, :anchor=>:w)

    sep = Tk::BWidget::Separator.new(parent, :orient=>:horizontal)
    button = TkButton.new(parent, :text=>'Show', :command=>proc{_show_msgdlg})

    button.pack(:side=>:bottom)
    sep.pack(:side=>:bottom, :fill=>:x, :pady=>10)
    Tk.pack(labf1, labf2, :side=>:left, :padx=>4, :anchor=>:n)
  end

  def self._stddlg(parent)
    Tk.pack(TkButton.new(parent, :text=>'Select a color '){|w|
              command(proc{DemoDialog._show_color(w)})
            },
            TkButton.new(parent, :text=>'Font selector dialog',
                         :command=>proc{_show_fontdlg}),
            TkButton.new(parent, :text=>'Progression dialog',
                         :command=>proc{_show_progdlg}),
            TkButton.new(parent, :text=>'Password dialog',
                         :command=>proc{_show_passdlg}),
            :side=>:left, :padx=>5, :anchor=>:w)
  end

  def self._show_color(w)
    dlg = Tk::BWidget::SelectColor.new(w, :color=>w.background)
    color = dlg.menu([:below, w])
    unless color.empty?
      w.background(color)
    end
  end

  def self._show_tmpldlg
    dlg = Tk::BWidget::Dialog.new(:relative=>Tk.root, :modal=>:local,
                                  :separator=>true, :title=>'Template dialog',
                                  :side=>@@tmpl['side'],
                                  :anchor=>@@tmpl['anchor'],
                                  :default=>0, :cancel=>1)
    dlg.add('name'=>'ok')
    dlg.add('name'=>'cancel')
    TkMessage.new(dlg.get_frame, :text=>"Template\nDialog", :justify=>:center,
                  :anchor=>:c, :width=>80).pack(:fill=>:both, :expand=>true,
                                                :padx=>100, :pady=>100)
    dlg.draw
    dlg.destroy
  end

  def self._show_msgdlg
    @@msgdlg.destroy if @@msgdlg
    @@msgdlg = Tk::BWidget::MessageDlg.new(:relative=>Tk.root,
                                           :message=>'Message for MessageBox',
                                           :type=>@@msg['type'],
                                           :icon=>@@msg['icon'],
                                           :buttons=>@@msg['buttons'])
    @@msgdlg.create
  end

  def self._show_fontdlg
    font = Tk::BWidget::SelectFont.new(:relative=>Tk.root,
                                       :font=>DemoVar.font).create
    DemoVar::Demo.update_font(font)
  end

  def self._show_progdlg
    @@progmsg.value = "Compute in progress..."
    @@progval.value = 0

    @@progdlg = Tk::BWidget::ProgressDlg.new(:relative=>Tk.root,
                                             :title=>'Wait...',
                                             :type=>'infinite', :width=>20,
                                             :textvariable=>@@progmsg,
                                             :variable=>@@progval,
                                             :stop=>'Stop') {
      command(proc{self.destroy})
      create
    }
    _update_progdlg
  end

  def self._update_progdlg
    TkTimer.new(20, -1, proc{
                  if @@progdlg && @@progdlg.winfo_exist?
                    @@progval.value = 2
                  else
                    stop
                  end
                }).start
  end

  def self._show_passdlg
    Tk::BWidget::PasswdDlg.new(:relative=>Tk.root).create
  end
end

