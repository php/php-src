#
#  TkImg - format 'window'
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'

# call setup script for general 'tkextlib' libraries
require 'tkextlib/setup.rb'

# call setup script
require 'tkextlib/tkimg/setup.rb'

# TkPackage.require('img::window', '1.3')
TkPackage.require('img::window')

module Tk
  module Img
    module WINDOW
      PACKAGE_NAME = 'img::window'.freeze
      def self.package_name
        PACKAGE_NAME
      end

      def self.package_version
        begin
          TkPackage.require('img::window')
        rescue
          ''
        end
      end
    end
  end
end
