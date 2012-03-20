#
#  tkextlib/tcllib/datefield.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * Tk datefield widget
#
# (The following is the original description of the library.)
#
# The datefield package provides the datefield widget which is an enhanced
# text entry widget for the purpose of date entry. Only valid dates of the
# form MM/DD/YYYY can be entered.
#
# The datefield widget is, in fact, just an entry widget with specialized
# bindings. This means all the command and options for an entry widget apply
# equally here.

require 'tk'
require 'tk/entry'
require 'tkextlib/tcllib.rb'

# TkPackage.require('datefield', '0.1')
TkPackage.require('datefield')

module Tk
  module Tcllib
    class Datefield < Tk::Entry
      PACKAGE_NAME = 'datefield'.freeze
      def self.package_name
        PACKAGE_NAME
      end

      def self.package_version
        begin
          TkPackage.require('datefield')
        rescue
          ''
        end
      end
    end
    DateField = Datefield
  end
end

class Tk::Tcllib::Datefield
  TkCommandNames = ['::datefield::datefield'.freeze].freeze

  def create_self(keys)
    if keys and keys != None
      tk_call_without_enc(self.class::TkCommandNames[0], @path,
                          *hash_kv(keys, true))
    else
      tk_call_without_enc(self.class::TkCommandNames[0], @path)
    end
  end
  private :create_self
end
