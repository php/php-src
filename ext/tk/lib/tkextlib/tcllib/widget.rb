#
#  tkextlib/tcllib/widget.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * megawidget package that uses snit as the object system (snidgets)
#

require 'tk'
require 'tkextlib/tcllib.rb'

# TkPackage.require('widget', '3.0')
TkPackage.require('widget')

module Tk::Tcllib
  module Widget
    PACKAGE_NAME = 'widget'.freeze
    def self.package_name
      PACKAGE_NAME
    end

    def self.package_version
      begin
        TkPackage.require('widget')
      rescue
        ''
      end
    end

    #--- followings may be private functions of tklib
    def self.isa(compare_as, *args)
      begin
        return Tk.tk_call('::widget::isa', compare_as, *args)
      rescue => e
        if TkComm.bool(Tk.tk_call('info','command','::widget::isa')) ||
            ! TkComm.bool(Tk.tk_call('info','command','::widget::validate'))
          fail e
        end
      end
      Tk.tk_call('::widget::validate', compare_as, *args)
    end
    def self.validate(compare_as, *args)
      begin
        return Tk.tk_call('::widget::validate', compare_as, *args)
      rescue => e
        if TkComm.bool(Tk.tk_call('info','command','::widget::validate')) ||
            ! TkComm.bool(Tk.tk_call('info','command','::widget::isa'))
          fail e
        end
      end
      Tk.tk_call('::widget::isa', compare_as, *args)
    end
  end
end

module Tk::Tcllib::Widget
  TkComm::TkExtlibAutoloadModule.unshift(self)

  autoload :Calendar,           'tkextlib/tcllib/calendar'

  autoload :Canvas_Sqmap,       'tkextlib/tcllib/canvas_sqmap'
  autoload :Canvas_Zoom,        'tkextlib/tcllib/canvas_zoom'

  autoload :Dialog,             'tkextlib/tcllib/dialog'

  autoload :Panelframe,         'tkextlib/tcllib/panelframe'
  autoload :PanelFrame,         'tkextlib/tcllib/panelframe'

  autoload :Ruler,              'tkextlib/tcllib/ruler'

  autoload :Screenruler,        'tkextlib/tcllib/screenruler'
  autoload :ScreenRuler,        'tkextlib/tcllib/screenruler'

  autoload :Scrolledwindow,     'tkextlib/tcllib/scrollwin'
  autoload :ScrolledWindow,     'tkextlib/tcllib/scrollwin'

  autoload :Superframe,         'tkextlib/tcllib/superframe'
  autoload :SuperFrame,         'tkextlib/tcllib/superframe'

  autoload :Toolbar,            'tkextlib/tcllib/toolbar'
  autoload :ToolbarItem,        'tkextlib/tcllib/toolbar'
end
