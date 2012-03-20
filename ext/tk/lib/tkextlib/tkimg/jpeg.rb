#
#  TkImg - format 'jpeg'
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'

# call setup script for general 'tkextlib' libraries
require 'tkextlib/setup.rb'

# call setup script
require 'tkextlib/tkimg/setup.rb'

# TkPackage.require('img::jpeg', '1.3')
TkPackage.require('img::jpeg')

module Tk
  module Img
    module JPEG
      PACKAGE_NAME = 'img::jpeg'.freeze
      def self.package_name
        PACKAGE_NAME
      end

      def self.package_version
        begin
          TkPackage.require('img::jpeg')
        rescue
          ''
        end
      end
    end
  end
end
