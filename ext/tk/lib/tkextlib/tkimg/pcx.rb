#
#  TkImg - format 'pcx'
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)#

require 'tk'

# call setup script for general 'tkextlib' libraries
require 'tkextlib/setup.rb'

# call setup script
require 'tkextlib/tkimg/setup.rb'

# TkPackage.require('img::pcx', '1.3')
TkPackage.require('img::pcx')

module Tk
  module Img
    module PCX
      PACKAGE_NAME = 'img::pcx'.freeze
      def self.package_name
        PACKAGE_NAME
      end

      def self.package_version
        begin
          TkPackage.require('img::pcx')
        rescue
          ''
        end
      end
    end
  end
end
