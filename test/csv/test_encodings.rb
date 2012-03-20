#!/usr/bin/env ruby -w
# encoding: UTF-8

# tc_encodings.rb
#
#  Created by James Edward Gray II on 2008-09-13.
#  Copyright 2008 James Edward Gray II. You can redistribute or modify this code
#  under the terms of Ruby's license.

require_relative "base"

class TestCSV::Encodings < TestCSV
  extend DifferentOFS

  def setup
    super
    require 'tempfile'
    @temp_csv_file = Tempfile.new(%w"test_csv. .csv")
    @temp_csv_path = @temp_csv_file.path
    @temp_csv_file.close
  end

  def teardown
    @temp_csv_file.close!
    super
  end

  ########################################
  ### Hand Test Some Popular Encodings ###
  ########################################

  def test_parses_utf8_encoding
    assert_parses( [ %w[ one two … ],
                     %w[ 1   …   3 ],
                     %w[ …   5   6 ] ], "UTF-8" )
  end

  def test_parses_latin1_encoding
    assert_parses( [ %w[ one    two    Résumé ],
                     %w[ 1      Résumé 3      ],
                     %w[ Résumé 5      6      ] ], "ISO-8859-1" )
  end

  def test_parses_utf16be_encoding
    assert_parses( [ %w[ one two … ],
                     %w[ 1   …   3 ],
                     %w[ …   5   6 ] ], "UTF-16BE" )
  end

  def test_parses_shift_jis_encoding
    assert_parses( [ %w[ 一 二 三 ],
                     %w[ 四 五 六 ],
                     %w[ 七 八 九 ] ], "Shift_JIS" )
  end

  ###########################################################
  ### Try Simple Reading for All Non-dummy Ruby Encodings ###
  ###########################################################

  def test_reading_with_most_encodings
    each_encoding do |encoding|
      begin
        assert_parses( [ %w[ abc def ],
                         %w[ ghi jkl ] ], encoding )
      rescue Encoding::ConverterNotFoundError
        fail("Failed to support #{encoding.name}.")
      end
    end
  end

  def test_regular_expression_escaping
    each_encoding do |encoding|
      begin
        assert_parses( [ %w[ abc def ],
                         %w[ ghi jkl ] ], encoding, col_sep: "|" )
      rescue Encoding::ConverterNotFoundError
        fail("Failed to properly escape #{encoding.name}.")
      end
    end
  end

  def test_read_with_default_encoding
    data             = "abc"
    default_external = Encoding.default_external
    each_encoding do |encoding|
      File.open(@temp_csv_path, "wb", encoding: encoding) {|f| f << data}
      begin
        no_warnings do
          Encoding.default_external = encoding
        end
        result = CSV.read(@temp_csv_path)[0][0]
      ensure
        no_warnings do
          Encoding.default_external = default_external
        end
      end
      assert_equal(encoding, result.encoding)
    end
  end

  #######################################################################
  ### Stress Test ASCII Compatible and Non-ASCII Compatible Encodings ###
  #######################################################################

  def test_auto_line_ending_detection
    # arrange data to place a \r at the end of CSV's read ahead point
    encode_for_tests([["a" * 509]], row_sep: "\r\n") do |data|
      assert_equal("\r\n".encode(data.encoding), CSV.new(data).row_sep)
    end
  end

  def test_csv_chars_are_transcoded
    encode_for_tests([%w[abc def]]) do |data|
      %w[col_sep row_sep quote_char].each do |csv_char|
        assert_equal( "|".encode(data.encoding),
                      CSV.new(data, csv_char.to_sym => "|").send(csv_char) )
      end
    end
  end

  def test_parser_works_with_encoded_headers
    encode_for_tests([%w[one two three], %w[1 2 3]]) do |data|
      parsed = CSV.parse(data, headers: true)
      assert( parsed.headers.all? { |h| h.encoding == data.encoding },
              "Wrong data encoding." )
      parsed.each do |row|
        assert( row.fields.all? { |f| f.encoding == data.encoding },
                "Wrong data encoding." )
      end
    end
  end

  def test_built_in_converters_transcode_to_utf_8_then_convert
    encode_for_tests([%w[one two three], %w[1 2 3]]) do |data|
      parsed = CSV.parse(data, converters: :integer)
      assert( parsed[0].all? { |f| f.encoding == data.encoding },
              "Wrong data encoding." )
      assert_equal([1, 2, 3], parsed[1])
    end
  end

  def test_built_in_header_converters_transcode_to_utf_8_then_convert
    encode_for_tests([%w[one two three], %w[1 2 3]]) do |data|
      parsed = CSV.parse( data, headers:           true,
                                header_converters: :downcase )
      assert( parsed.headers.all? { |h| h.encoding.name == "UTF-8" },
              "Wrong data encoding." )
      assert( parsed[0].fields.all? { |f| f.encoding == data.encoding },
              "Wrong data encoding." )
    end
  end

  def test_open_allows_you_to_set_encodings
    encode_for_tests([%w[abc def]]) do |data|
      # read and write in encoding
      File.open(@temp_csv_path, "wb:#{data.encoding.name}") { |f| f << data }
      CSV.open(@temp_csv_path, "rb:#{data.encoding.name}") do |csv|
        csv.each do |row|
          assert( row.all? { |f| f.encoding == data.encoding },
                  "Wrong data encoding." )
        end
      end

      # read and write with transcoding
      File.open(@temp_csv_path, "wb:UTF-32BE:#{data.encoding.name}") do |f|
        f << data
      end
      CSV.open(@temp_csv_path, "rb:UTF-32BE:#{data.encoding.name}") do |csv|
        csv.each do |row|
          assert( row.all? { |f| f.encoding == data.encoding },
                  "Wrong data encoding." )
        end
      end
    end
  end

  def test_foreach_allows_you_to_set_encodings
    encode_for_tests([%w[abc def]]) do |data|
      # read and write in encoding
      File.open(@temp_csv_path, "wb", encoding: data.encoding) { |f| f << data }
      CSV.foreach(@temp_csv_path, encoding: data.encoding) do |row|
        row.each {|f| assert_equal(f.encoding, data.encoding)}
      end

      # read and write with transcoding
      File.open(@temp_csv_path, "wb:UTF-32BE:#{data.encoding.name}") do |f|
        f << data
      end
      CSV.foreach( @temp_csv_path,
                   encoding: "UTF-32BE:#{data.encoding.name}" ) do |row|
        assert( row.all? { |f| f.encoding == data.encoding },
                "Wrong data encoding." )
      end
    end
  end

  def test_read_allows_you_to_set_encodings
    encode_for_tests([%w[abc def]]) do |data|
      # read and write in encoding
      File.open(@temp_csv_path, "wb:#{data.encoding.name}") { |f| f << data }
      rows = CSV.read(@temp_csv_path, encoding: data.encoding.name)
      assert( rows.flatten.all? { |f| f.encoding == data.encoding },
              "Wrong data encoding." )

      # read and write with transcoding
      File.open(@temp_csv_path, "wb:UTF-32BE:#{data.encoding.name}") do |f|
        f << data
      end
      rows = CSV.read( @temp_csv_path,
                       encoding: "UTF-32BE:#{data.encoding.name}" )
      assert( rows.flatten.all? { |f| f.encoding == data.encoding },
              "Wrong data encoding." )
    end
  end

  #################################
  ### Write CSV in any Encoding ###
  #################################

  def test_can_write_csv_in_any_encoding
    each_encoding do |encoding|
      # test generate_line with encoding hint
      begin
        csv = %w[abc d|ef].map { |f| f.encode(encoding) }.
          to_csv(col_sep: "|", encoding: encoding.name)
      rescue Encoding::ConverterNotFoundError
        next
      end
      assert_equal(encoding, csv.encoding)

      # test generate_line with encoding guessing from fields
      csv = %w[abc d|ef].map { |f| f.encode(encoding) }.to_csv(col_sep: "|")
      assert_equal(encoding, csv.encoding)

      # writing to files
      data = encode_ary([%w[abc d,ef], %w[123 456 ]], encoding)
      CSV.open(@temp_csv_path, "wb:#{encoding.name}") do |f|
        data.each { |row| f << row }
      end
      assert_equal(data, CSV.read(@temp_csv_path, encoding: encoding.name))
    end
  end

  def test_encoding_is_upgraded_during_writing_as_needed
    data = ["foo".force_encoding("US-ASCII"), "\u3042"]
    assert_equal("US-ASCII", data.first.encoding.name)
    assert_equal("UTF-8",    data.last.encoding.name)
    assert_equal("UTF-8",    data.join('').encoding.name)
    assert_equal("UTF-8",    data.to_csv.encoding.name)
  end

  def test_encoding_is_upgraded_for_ascii_content_during_writing_as_needed
    data = ["foo".force_encoding("ISO-8859-1"), "\u3042"]
    assert_equal("ISO-8859-1", data.first.encoding.name)
    assert_equal("UTF-8",      data.last.encoding.name)
    assert_equal("UTF-8",      data.join('').encoding.name)
    assert_equal("UTF-8",      data.to_csv.encoding.name)
  end

  private

  def assert_parses(fields, encoding, options = { })
    encoding = Encoding.find(encoding) unless encoding.is_a? Encoding
    orig_fields = fields
    fields   = encode_ary(fields, encoding)
    data = ary_to_data(fields, options)
    parsed   = CSV.parse(data, options)
    assert_equal(fields, parsed)
    parsed.flatten.each_with_index do |field, i|
      assert_equal(encoding, field.encoding, "Field[#{i + 1}] was transcoded.")
    end
    File.open(@temp_csv_path, "wb") {|f| f.print(data)}
    CSV.open(@temp_csv_path, "rb:#{encoding}", options) do |csv|
      csv.each_with_index do |row, i|
        assert_equal(fields[i], row)
      end
    end
    begin
      CSV.open(@temp_csv_path, "rb:#{encoding}:#{__ENCODING__}", options) do |csv|
        csv.each_with_index do |row, i|
          assert_equal(orig_fields[i], row)
        end
      end unless encoding == __ENCODING__
    rescue Encoding::ConverterNotFoundError
    end
    options[:encoding] = encoding.name
    CSV.open(@temp_csv_path, options) do |csv|
      csv.each_with_index do |row, i|
        assert_equal(fields[i], row)
      end
    end
    options.delete(:encoding)
    options[:external_encoding] = encoding.name
    options[:internal_encoding] = __ENCODING__.name
    begin
      CSV.open(@temp_csv_path, options) do |csv|
        csv.each_with_index do |row, i|
          assert_equal(orig_fields[i], row)
        end
      end unless encoding == __ENCODING__
    rescue Encoding::ConverterNotFoundError
    end
  end

  def encode_ary(ary, encoding)
    ary.map { |row| row.map { |field| field.encode(encoding) } }
  end

  def ary_to_data(ary, options = { })
    encoding   = ary.flatten.first.encoding
    quote_char = (options[:quote_char] || '"').encode(encoding)
    col_sep    = (options[:col_sep]    || ",").encode(encoding)
    row_sep    = (options[:row_sep]    || "\n").encode(encoding)
    ary.map { |row|
      row.map { |field|
        [quote_char, field.encode(encoding), quote_char].join('')
      }.join(col_sep) + row_sep
    }.join('').encode(encoding)
  end

  def encode_for_tests(data, options = { })
    yield ary_to_data(encode_ary(data, "UTF-8"),    options)
    yield ary_to_data(encode_ary(data, "UTF-16BE"), options)
  end

  def each_encoding
    Encoding.list.each do |encoding|
      next if encoding.dummy?  # skip "dummy" encodings
      yield encoding
    end
  end
  
  def no_warnings
    old_verbose, $VERBOSE = $VERBOSE, nil
    yield
  ensure
    $VERBOSE = old_verbose
  end
end
