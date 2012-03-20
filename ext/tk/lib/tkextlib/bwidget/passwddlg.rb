#
#  tkextlib/bwidget/passwddlg.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/bwidget.rb'
require 'tkextlib/bwidget/messagedlg'

module Tk
  module BWidget
    class PasswdDlg < Tk::BWidget::MessageDlg
    end
  end
end

class Tk::BWidget::PasswdDlg
  TkCommandNames = ['PasswdDlg'.freeze].freeze
  WidgetClassName = 'PasswdDlg'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __strval_optkeys
    super() << 'loginhelptext' << 'loginlabel' << 'logintext' <<
      'passwdlabel' << 'passwdtext'
  end
  private :__strval_optkeys

  def __boolval_optkeys
    super() << 'passwdeditable' << 'homogeneous'
  end
  private :__boolval_optkeys

  def __tkvariable_optkeys
    super() << 'loginhelpvar' << 'logintextvariable' <<
      'passwdhelpvar' << 'passwdtextvariable'
  end
  private :__tkvariable_optkeys

  def create
    login, passwd = simplelist(tk_call(self.class::TkCommandNames[0],
                                       @path, *hash_kv(@keys)))
    [login, passwd]
  end
end
