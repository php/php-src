#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/bwidget'

module DemoVar
  @_wfont    = nil
  @notebook  = nil
  @mainframe = nil
  @status    = TkVariable.new
  @prgtext   = TkVariable.new
  @prgindic  = TkVariable.new
  @font      = nil
  @font_name = nil
  @toolbar1  = TkVariable.new(true)
  @toolbar2  = TkVariable.new(true)
end
class << DemoVar
  attr_accessor :_wfont, :notebook, :mainframe, :font, :font_name
  attr_reader   :status, :prgtext, :prgindic, :toolbar1, :toolbar2
end

class BWidget_Demo
  DEMODIR = File.dirname(File.expand_path(__FILE__))

  %w(manager basic select dnd tree tmpldlg).each{|f|
    require File.join(DEMODIR, f << '.rb')
  }

  def initialize
    TkOption.add('*TitleFrame.l.font', 'helvetica 11 bold italic')

    root = TkRoot.new(:title=>'BWidget demo')
    root.withdraw

    _create

    Tk::BWidget.place(root, 0, 0, :center)
    root.deiconify
    root.raise
    root.focus(true)

    root.geometry(root.geometry)
  end

  def _create
    DemoVar.prgtext.value = 'Please wait while loading font...'
    DemoVar.prgindic.value = -1

    intro = _create_intro

    Tk.update

    Tk::BWidget::SelectFont.load_font

    descmenu = [
      '&File', 'all', 'file', 0, [
        ['command', 'E&xit', [], 'Exit BWidget demo', [],
          {:command=>proc{exit}}]
      ],
      '&Options', 'all', 'options', 0, [
        ['checkbutton', 'Toolbar &1', ['all', 'option'],
          'Show/hide toolbar 1', [],
          { :variable=>DemoVar.toolbar1,
            :command=>proc{
              DemoVar.mainframe.show_toolbar(0, DemoVar.toolbar1.value)
            }
          }
        ],
        ['checkbutton', 'Toolbar &2', ['all', 'option'],
          'Show/hide toolbar 2', [],
          { :variable=>DemoVar.toolbar2,
            :command=>proc{
              DemoVar.mainframe.show_toolbar(1, DemoVar.toolbar2.value)
            }
          }
        ]
      ]
    ]

    DemoVar.prgtext.value = 'Creating MainFrame...'
    DemoVar.prgindic.value = 0

    DemoVar.mainframe = Tk::BWidget::MainFrame.new(
                                :menu=>descmenu,
                                :textvariable=>DemoVar.status,
                                :progressvar=>DemoVar.prgindic
                        )

    # toobar 1 creation
    DemoVar.prgindic.numeric += 1

    DemoVar.mainframe.add_toolbar{|tb1|
      Tk::BWidget::ButtonBox.new(tb1, :spacing=>0, :padx=>1, :pady=>1){|bbox|
        add(:image=>Tk::BWidget::Bitmap.new('new'),
            :highlightthickness=>0, :takefocus=>0, :relief=>:link,
            :borderwidth=>1, :padx=>1, :pady=>1,
            :command=>proc{puts 'select "Create a new file" icon'},
            :helptext=>"Create a new file")

        add(:image=>Tk::BWidget::Bitmap.new('open'),
            :highlightthickness=>0, :takefocus=>0, :relief=>:link,
            :borderwidth=>1, :padx=>1, :pady=>1,
            :command=>proc{puts 'select "Open an existing file" icon'},
            :helptext=>"Open an existing file")

        add(:image=>Tk::BWidget::Bitmap.new('save'),
            :highlightthickness=>0, :takefocus=>0, :relief=>:link,
            :borderwidth=>1, :padx=>1, :pady=>1,
            :command=>proc{puts 'select "Save file" icon'},
            :helptext=>"Save file")

        pack(:side=>:left, :anchor=>:w)
      }

      Tk::BWidget::Separator.new(tb1, :orient=>:vertical){
        pack(:side=>:left, :fill=>:y, :padx=>4, :anchor=>:w)
      }

      DemoVar.prgindic.numeric += 1

      Tk::BWidget::ButtonBox.new(tb1, :spacing=>0, :padx=>1, :pady=>1){|bbox|
        add(:image=>Tk::BWidget::Bitmap.new('cut'),
            :highlightthickness=>0, :takefocus=>0, :relief=>:link,
            :borderwidth=>1, :padx=>1, :pady=>1,
            :command=>proc{puts 'select "Cut selection" icon'},
            :helptext=>"Cut selection")

        add(:image=>Tk::BWidget::Bitmap.new('copy'),
            :highlightthickness=>0, :takefocus=>0, :relief=>:link,
            :borderwidth=>1, :padx=>1, :pady=>1,
            :command=>proc{puts 'select "Copy selection" icon'},
            :helptext=>"Copy selection")

        add(:image=>Tk::BWidget::Bitmap.new('paste'),
            :highlightthickness=>0, :takefocus=>0, :relief=>:link,
            :borderwidth=>1, :padx=>1, :pady=>1,
            :command=>proc{puts 'select "Paste selection" icon'},
            :helptext=>"Paste selection")

        pack(:side=>:left, :anchor=>:w)
      }
    }

    # toolbar 2 creation
    DemoVar.prgindic.numeric += 1

    tb2 = DemoVar.mainframe.add_toolbar
    DemoVar._wfont = Tk::BWidget::SelectFont::Toolbar.new(tb2,
                       :command=>proc{update_font(DemoVar._wfont[:font])}
                     )
    DemoVar.font = DemoVar._wfont[:font]
    DemoVar._wfont.pack(:side=>:left, :anchor=>:w)

    DemoVar.mainframe.add_indicator(
      :text=>"BWidget #{Tk::BWidget.package_version}"
    )
    DemoVar.mainframe.add_indicator(:textvariable=>'tk_patchLevel')

    # NoteBook creation
    DemoVar.notebook = Tk::BWidget::NoteBook.new(DemoVar.mainframe.get_frame)

    DemoVar.prgtext.value = "Creating Manager..."
    DemoVar.prgindic.numeric += 1
    DemoManager.create(DemoVar.notebook)

    DemoVar.prgtext.value = "Creating Basic..."
    DemoVar.prgindic.numeric += 1
    DemoBasic.create(DemoVar.notebook)

    DemoVar.prgtext.value = "Creating Select..."
    DemoVar.prgindic.numeric += 1
    DemoSelect.create(DemoVar.notebook)

    DemoVar.prgtext.value = "Creating Dialog..."
    DemoVar.prgindic.numeric += 1
    DemoDialog.create(DemoVar.notebook)

    DemoVar.prgtext.value = "Creating Drag and Drop..."
    DemoVar.prgindic.numeric += 1
    DemoDnD.create(DemoVar.notebook)

    DemoVar.prgtext.value = "Creating Tree..."
    DemoVar.prgindic.numeric += 1
    DemoTree.create(DemoVar.notebook)

    DemoVar.prgtext.value = "Done"
    DemoVar.prgindic.numeric += 1

    DemoVar.notebook.compute_size
    DemoVar.notebook.pack(:fill=>:both, :expand=>true, :padx=>4, :pady=>4)
    DemoVar.notebook.raise(DemoVar.notebook.get_page(0))

    DemoVar.mainframe.pack(:fill=>:both, :expand=>true)

    Tk.update_idletasks

    intro.destroy
  end

  def update_font(newfont)
    root = Tk.root
    root[:cursor] = 'watch'
    if newfont != '' && DemoVar.font != newfont
      DemoVar._wfont[:font] = newfont
      DemoVar.notebook[:font] = newfont
      DemoVar.font = newfont
    end
    root[:cursor] = ''
  end

  def _create_intro
    top = TkToplevel.new(:relief=>:raised, :borderwidth=>2)
    top.withdraw
    top.overrideredirect(true)

    ximg  = TkLabel.new(top, :bitmap=>"@#{File.join(DEMODIR,'x1.xbm')}",
                        :foreground=>'grey90', :background=>'white')
    bwimg = TkLabel.new(ximg, :bitmap=>"@#{File.join(DEMODIR,'bwidget.xbm')}",
                        :foreground=>'grey90', :background=>'white')
    frame = TkFrame.new(ximg, :background=>'white')
    TkLabel.new(frame, :text=>'Loading demo',
                :background=>'white', :font=>'times 8').pack
    TkLabel.new(frame, :textvariable=>DemoVar.prgtext,
                :background=>'white', :font=>'times 8', :width=>35).pack
    Tk::BWidget::ProgressBar.new(frame, :width=>50, :height=>10,
                                 :background=>'white',
                                 :variable=>DemoVar.prgindic,
                                 :maximum=>10).pack
    frame.place(:x=>0, :y=>0, :anchor=>:nw)
    bwimg.place(:relx=>1, :rely=>1, :anchor=>:se)
    ximg.pack
    Tk::BWidget.place(top, 0, 0, :center)
    top.deiconify

    top
  end
end

module DemoVar
  Demo = BWidget_Demo.new
end

Tk.mainloop
