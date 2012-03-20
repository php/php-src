#
#  TkImg - format 'tiff'
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'

# call setup script for general 'tkextlib' libraries
require 'tkextlib/setup.rb'

# call setup script
require 'tkextlib/tkimg/setup.rb'

# TkPackage.require('img::tiff', '1.3')
TkPackage.require('img::tiff')

module Tk
  module Img
    module TIFF
      PACKAGE_NAME = 'img::tiff'.freeze
      def self.package_name
        PACKAGE_NAME
      end

      def self.package_version
        begin
          TkPackage.require('img::tiff')
        rescue
          ''
        end
      end
    end
  end
end
