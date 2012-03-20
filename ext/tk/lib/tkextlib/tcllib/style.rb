#
#  tkextlib/tcllib/style.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * select and use some 'style' of option (resource) DB
#

require 'tk'
require 'tkextlib/tcllib.rb'

module Tk::Tcllib
  module Style
    PACKAGE_NAME = 'style'.freeze
    def self.package_name
      PACKAGE_NAME
    end

    def self.package_version
      begin
        TkPackage.require('style')
      rescue
        ''
      end
    end

    def self.not_available
      fail RuntimeError, "'tkextlib/tcllib/style' extension is not available on your current environment."
    end

    def self.names
      Tk::Tcllib::Style.not_available
    end

    def self.use(style)
      Tk::Tcllib::Style.not_available
    end
  end
end

# TkPackage.require('style', '0.1')
# TkPackage.require('style', '0.3')
TkPackage.require('style')

module Tk::Tcllib
  class << Style
    undef not_available
  end

  module Style
    extend TkCore

    def self.names
      tk_split_simplelist(tk_call('style::names'))
    end

    def self.use(style)
      tk_call('style::use', style)
    end
  end
end
