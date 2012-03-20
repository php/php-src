#
#  TkImg - format 'sgi'
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'

# call setup script for general 'tkextlib' libraries
require 'tkextlib/setup.rb'

# call setup script
require 'tkextlib/tkimg/setup.rb'

# TkPackage.require('img::sgi', '1.3')
TkPackage.require('img::sgi')

module Tk
  module Img
    module SGI
      PACKAGE_NAME = 'img::sgi'.freeze
      def self.package_name
        PACKAGE_NAME
      end

      def self.package_version
        begin
          TkPackage.require('img::sgi')
        rescue
          ''
        end
      end
    end
  end
end
