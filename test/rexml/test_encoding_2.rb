require "test/unit/testcase"

require "rexml/document"
require "rexml/parseexception"

=begin
# THIS DOESN'T WORK
begin
  require 'iconv'

  UnixCharsets = open("| iconv -l") do |f|
     f.readlines[5..-1].collect { |x| x.sub(/\/\/\n/,"").delete(' ') }
  end

  DATA = <<END
<?xml version="1.0" encoding='ENC'?>
<Ruby xmlns="http://www.ruby-lang.org/ruby/1.8">
</Ruby>
END


  class IConvTester < Test::Unit::TestCase

    def test_iconv
      broken_encodings = 0
      UnixCharsets.each do |enc|
        begin
          puts "Testing encoding #{enc}"
          data = DATA.dup
          data[/ENC/] = enc
          REXML::Document.new(data).root
        rescue REXML::ParseException => e
          broken_encodings += 1
          fail "Encoding #{enc} does not work with REXML: #{e.message}"
        rescue Errno::EINVAL => e
          broken_encodings += 1
          fail "Encoding #{enc} does not work with REXML: #{e.message}"
        rescue ArgumentError => e
          broken_encodings += 1
          fail "Encoding #{enc} does not work with REXML: #{e.message}"
        rescue
          broken_encodings += 1
          fail "Encoding #{enc} does not work with REXML: #{$!.message}"
        end
      end

      if broken_encodings > 0
        fail "There were #{broken_encodings} encoding failures out of #{UnixCharsets.size} plus some REXML internal encodings"
      else
        fail "There were no encoding failures"
      end

      puts "Full list of registered encodings in REXML:"
      puts REXML::Encoding::ENCODING_CLAIMS.values.join(', ')
    end
  end
rescue LoadError
end
=end
