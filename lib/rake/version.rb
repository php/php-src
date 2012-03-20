module Rake
  VERSION = '0.9.2.2'

  module Version # :nodoc: all
    MAJOR, MINOR, BUILD = VERSION.split '.'
    NUMBERS = [ MAJOR, MINOR, BUILD ]
  end
end
