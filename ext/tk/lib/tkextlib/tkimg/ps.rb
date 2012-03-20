#
#  TkImg - format 'ps'
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'

# call setup script for general 'tkextlib' libraries
require 'tkextlib/setup.rb'

# call setup script
require 'tkextlib/tkimg/setup.rb'

# TkPackage.require('img::ps', '1.3')
TkPackage.require('img::ps')

module Tk
  module Img
    module PS
      PACKAGE_NAME = 'img::ps'.freeze
      def self.package_name
        PACKAGE_NAME
      end

      def self.package_version
        begin
          TkPackage.require('img::ps')
        rescue
          ''
        end
      end
    end
  end
end
