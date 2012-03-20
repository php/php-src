#
#  BWidget extension support
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'

# call setup script for general 'tkextlib' libraries
require 'tkextlib/setup.rb'

# call setup script
require 'tkextlib/bwidget/setup.rb'

# load all image format handlers
#TkPackage.require('BWidget', '1.7')
TkPackage.require('BWidget')

module Tk
  module BWidget
    TkComm::TkExtlibAutoloadModule.unshift(self)
    # Require autoload-symbols which is a same name as widget classname.
    # Those are used at  TkComm._genobj_for_tkwidget method.

    extend TkCore

    LIBRARY = tk_call('set', '::BWIDGET::LIBRARY')

    PACKAGE_NAME = 'BWidget'.freeze
    def self.package_name
      PACKAGE_NAME
    end

    def self.package_version
      begin
        TkPackage.require('BWidget')
      rescue
        ''
      end
    end

    def self.XLFDfont(cmd, *args)
      if args[-1].kind_of?(Hash)
        keys = args.pop
        args.concat(hash_kv(keys))
      end
      tk_call('BWidget::XLFDfont', cmd, *args)
    end

    def self.assert(exp, msg=None)
      tk_call('BWidget::assert', exp, msg)
    end

    def self.badOptionString(type, value, list)
      tk_call('BWidget::badOptionString', type, value, list)
    end

    def self.bindMouseWheel(widget)
      tk_call('BWidget::bindMouseWheel', widget)
    end

    def self.classes(klass)
      list(tk_call('BWidget::classes', klass))
    end

    def self.clonename(menu)
      tk_call('BWidget::clonename', menu)
    end

    def self.focus(opt, path)
      tk_call('BWidget::focus', opt, path)
    end

    def self.get3dcolor(path, bgcolor)
      tk_call('BWidget::get3dcolor', path, bgcolor)
    end

    def self.getname(name)
      tk_call('BWidget::getname', name)
    end

    def self.grab(opt, path)
      tk_call('BWidget::grab', opt, path)
    end

    def self.inuse(klass)
      bool(tk_call('BWidget::inuse', klass))
    end

    def self.library(klass, *klasses)
      tk_call('BWidget::library', klass, *klasses)
    end

    def self.lreorder(list, neworder)
      tk_call('BWidget::lreorder', list, neworder)
    end

    def self.parsetext(text)
      tk_call('BWidget::parsetext', text)
    end

    def self.place(path, w, h, *args)
      if args[-1].kind_of?(Hash)
        keys = args.pop
        args.concat(hash_kv(keys))
      end
      tk_call('BWidget::place', path, w, h, *(args.flatten))
    end

    def self.write(file, mode=None)
      tk_call('BWidget::write', file, mode)
    end

    def self.wrongNumArgsString(str)
      tk_call('BWidget::wrongNumArgsString', str)
    end

    ####################################################

    autoload :ArrowButton,     'tkextlib/bwidget/arrowbutton'
    autoload :Bitmap,          'tkextlib/bwidget/bitmap'
    autoload :Button,          'tkextlib/bwidget/button'
    autoload :ButtonBox,       'tkextlib/bwidget/buttonbox'
    autoload :ComboBox,        'tkextlib/bwidget/combobox'
    autoload :Dialog,          'tkextlib/bwidget/dialog'
    autoload :DragSite,        'tkextlib/bwidget/dragsite'
    autoload :DropSite,        'tkextlib/bwidget/dropsite'
    autoload :DynamicHelp,     'tkextlib/bwidget/dynamichelp'
    autoload :Entry,           'tkextlib/bwidget/entry'
    autoload :Label,           'tkextlib/bwidget/label'
    autoload :LabelEntry,      'tkextlib/bwidget/labelentry'
    autoload :LabelFrame,      'tkextlib/bwidget/labelframe'
    autoload :ListBox,         'tkextlib/bwidget/listbox'
    autoload :MainFrame,       'tkextlib/bwidget/mainframe'
    autoload :MessageDlg,      'tkextlib/bwidget/messagedlg'
    autoload :NoteBook,        'tkextlib/bwidget/notebook'
    autoload :PagesManager,    'tkextlib/bwidget/pagesmanager'
    autoload :PanedWindow,     'tkextlib/bwidget/panedwindow'
    autoload :PasswdDlg,       'tkextlib/bwidget/passwddlg'
    autoload :ProgressBar,     'tkextlib/bwidget/progressbar'
    autoload :ProgressDlg,     'tkextlib/bwidget/progressdlg'
    autoload :ScrollableFrame, 'tkextlib/bwidget/scrollableframe'
    autoload :ScrolledWindow,  'tkextlib/bwidget/scrolledwindow'
    autoload :ScrollView,      'tkextlib/bwidget/scrollview'
    autoload :SelectColor,     'tkextlib/bwidget/selectcolor'
    autoload :SelectFont,      'tkextlib/bwidget/selectfont'
    autoload :Separator,       'tkextlib/bwidget/separator'
    autoload :SpinBox,         'tkextlib/bwidget/spinbox'
    autoload :TitleFrame,      'tkextlib/bwidget/titleframe'
    autoload :Tree,            'tkextlib/bwidget/tree'
    autoload :Widget,          'tkextlib/bwidget/widget'

  end
end
