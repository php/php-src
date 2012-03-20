#
#  tkextlib/tcllib/ip_entry.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * An IP address entry widget
#
# (The following is the original description of the library.)
#
# This package provides a widget for the entering of a IP address.
# It guarantees a valid address at all times.

require 'tk'
require 'tkextlib/tcllib.rb'

# TkPackage.require('ipentry', '0.1')
TkPackage.require('ipentry')

module Tk
  module Tcllib
    class IP_Entry < Tk::Entry
      PACKAGE_NAME = 'ipentry'.freeze
      def self.package_name
        PACKAGE_NAME
      end

      def self.package_version
        begin
          TkPackage.require('ipentry')
        rescue
          ''
        end
      end
    end
    IPEntry = IP_Entry

    class IP_Entry6 < IP_Entry
    end
    IPEntry6 = IP_Entry6
    IP6_Entry = IP_Entry6
  end
end

class Tk::Tcllib::IP_Entry
  TkCommandNames = ['::ipentry::ipentry'.freeze].freeze
  WidgetClassName = 'IPEntry'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def create_self(keys)
    if keys and keys != None
      tk_call_without_enc(self.class::TkCommandNames[0], @path,
                          *hash_kv(keys, true))
    else
      tk_call_without_enc(self.class::TkCommandNames[0], @path)
    end
  end
  private :create_self

  def __strval_optkeys
    super() << 'fg' << 'bg' << 'insertbackground'
  end
  private :__strval_optkeys

  def complete?
    bool(tk_send_without_enc('complete'))
  end

  def insert(*ip)
    tk_send_without_enc('insert', array2tk_list(ip.flatten))
  end
end

class Tk::Tcllib::IP_Entry6 < Tk::Tcllib::IP_Entry
  TkCommandNames = ['::ipentry::ipentry6'.freeze].freeze
end
