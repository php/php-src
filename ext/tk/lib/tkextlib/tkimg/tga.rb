#
#  TkImg - format 'tga'
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'

# call setup script for general 'tkextlib' libraries
require 'tkextlib/setup.rb'

# call setup script
require 'tkextlib/tkimg/setup.rb'

# TkPackage.require('img::tga', '1.3')
TkPackage.require('img::tga')

module Tk
  module Img
    module TGA
      PACKAGE_NAME = 'img::tga'.freeze
      def self.package_name
        PACKAGE_NAME
      end

      def self.package_version
        begin
          TkPackage.require('img::tga')
        rescue
          ''
        end
      end
    end
  end
end
