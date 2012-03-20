#
#  TkImg - format 'xbm'
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'

# call setup script for general 'tkextlib' libraries
require 'tkextlib/setup.rb'

# call setup script
require 'tkextlib/tkimg/setup.rb'

# TkPackage.require('img::xbm', '1.3')
TkPackage.require('img::xbm')

module Tk
  module Img
    module XBM
      PACKAGE_NAME = 'img::xbm'.freeze
      def self.package_name
        PACKAGE_NAME
      end

      def self.package_version
        begin
          TkPackage.require('img::xbm')
        rescue
          ''
        end
      end
    end
  end
end
