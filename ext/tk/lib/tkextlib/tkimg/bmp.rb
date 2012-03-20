#
#  TkImg - format 'bmp'
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'

# call setup script for general 'tkextlib' libraries
require 'tkextlib/setup.rb'

# call setup script
require 'tkextlib/tkimg/setup.rb'

#TkPackage.require('img::bmp', '1.3')
TkPackage.require('img::bmp')

module Tk
  module Img
    module BMP
      PACKAGE_NAME = 'img::bmp'.freeze
      def self.package_name
        PACKAGE_NAME
      end

      def self.package_version
        begin
          TkPackage.require('img::bmp')
        rescue
          ''
        end
      end
    end
  end
end
