# -*- ruby -*-

require 'shellwords'
require 'optparse'

OptionParser.accept(Shellwords) {|s,| Shellwords.shellwords(s)}
