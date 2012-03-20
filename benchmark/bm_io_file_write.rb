#
# Seek and Write file.
#

require 'tempfile'

max = 200_000
str = "Hello world!  " * 1000
f = Tempfile.new('yarv-benchmark')

max.times{
  f.seek 0
  f.write str
}
