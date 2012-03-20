#
#  TkImg - format 'ppm'
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'

# call setup script for general 'tkextlib' libraries
require 'tkextlib/setup.rb'

# call setup script
require 'tkextlib/tkimg/setup.rb'

# TkPackage.require('img::ppm', '1.3')
TkPackage.require('img::ppm')

module Tk
  module Img
    module PPM
      PACKAGE_NAME = 'img::ppm'.freeze
      def self.package_name
        PACKAGE_NAME
      end

      def self.package_version
        begin
          TkPackage.require('img::ppm')
        rescue
          ''
        end
      end
    end
  end
end
