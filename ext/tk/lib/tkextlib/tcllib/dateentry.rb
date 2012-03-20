#
#  tkextlib/tcllib/dateentry.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * dateentry widget
#

require 'tk'
require 'tkextlib/tcllib.rb'

# TkPackage.require('widget::dateentry', '0.91')
TkPackage.require('widget::dateentry')

module Tk::Tcllib
  module Widget
    class Dateentry < Tk::Tile::TEntry
      PACKAGE_NAME = 'widget::dateentry'.freeze
      def self.package_name
        PACKAGE_NAME
      end

      def self.package_version
        begin
          TkPackage.require('widget::dateentry')
        rescue
          ''
        end
      end
    end
    DateEntry = Dateentry
  end
end

class Tk::Tcllib::Widget::Dateentry
  TkCommandNames = ['::widget::dateentry'.freeze].freeze

  def __strval_optkeys
    super() << ['dateformat']
  end
  private :__strval_optkeys

  def create_self(keys)
    if keys and keys != None
      tk_call_without_enc(self.class::TkCommandNames[0], @path,
                          *hash_kv(keys, true))
    else
      tk_call_without_enc(self.class::TkCommandNames[0], @path)
    end
  end
  private :create_self

  def post
    tk_send('post')
    self
  end

  def unpost
    tk_send('unpost')
    self
  end
end
