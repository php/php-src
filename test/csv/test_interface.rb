#!/usr/bin/env ruby -w
# encoding: UTF-8

# tc_interface.rb
#
#  Created by James Edward Gray II on 2005-10-31.
#  Copyright 2005 James Edward Gray II. You can redistribute or modify this code
#  under the terms of Ruby's license.

require_relative "base"

class TestCSV::Interface < TestCSV
  extend DifferentOFS

  def setup
    super
    @path = File.join(File.dirname(__FILE__), "temp_test_data.csv")

    File.open(@path, "wb") do |file|
      file << "1\t2\t3\r\n"
      file << "4\t5\r\n"
    end

    @expected = [%w{1 2 3}, %w{4 5}]
  end

  def teardown
    File.unlink(@path)
    super
  end

  ### Test Read Interface ###

  def test_foreach
    CSV.foreach(@path, col_sep: "\t", row_sep: "\r\n") do |row|
      assert_equal(@expected.shift, row)
    end
  end

  def test_open_and_close
    csv = CSV.open(@path, "r+", col_sep: "\t", row_sep: "\r\n")
    assert_not_nil(csv)
    assert_instance_of(CSV, csv)
    assert_equal(false, csv.closed?)
    csv.close
    assert(csv.closed?)

    ret = CSV.open(@path) do |new_csv|
      csv = new_csv
      assert_instance_of(CSV, new_csv)
      "Return value."
    end
    assert(csv.closed?)
    assert_equal("Return value.", ret)
  end

  def test_parse
    data = File.binread(@path)
    assert_equal( @expected,
                  CSV.parse(data, col_sep: "\t", row_sep: "\r\n") )

    CSV.parse(data, col_sep: "\t", row_sep: "\r\n") do |row|
      assert_equal(@expected.shift, row)
    end
  end

  def test_parse_line
    row = CSV.parse_line("1;2;3", col_sep: ";")
    assert_not_nil(row)
    assert_instance_of(Array, row)
    assert_equal(%w{1 2 3}, row)

    # shortcut interface
    row = "1;2;3".parse_csv(col_sep: ";")
    assert_not_nil(row)
    assert_instance_of(Array, row)
    assert_equal(%w{1 2 3}, row)
  end

  def test_parse_line_with_empty_lines
    assert_equal(nil,       CSV.parse_line(""))  # to signal eof
    assert_equal(Array.new, CSV.parse_line("\n1,2,3"))
  end

  def test_read_and_readlines
    assert_equal( @expected,
                  CSV.read(@path, col_sep: "\t", row_sep: "\r\n") )
    assert_equal( @expected,
                  CSV.readlines(@path, col_sep: "\t", row_sep: "\r\n") )


    data = CSV.open(@path, col_sep: "\t", row_sep: "\r\n") do |csv|
      csv.read
    end
    assert_equal(@expected, data)
    data = CSV.open(@path, col_sep: "\t", row_sep: "\r\n") do |csv|
      csv.readlines
    end
    assert_equal(@expected, data)
  end

  def test_table
    table = CSV.table(@path, col_sep: "\t", row_sep: "\r\n")
    assert_instance_of(CSV::Table, table)
    assert_equal([[:"1", :"2", :"3"], [4, 5, nil]], table.to_a)
  end

  def test_shift  # aliased as gets() and readline()
    CSV.open(@path, "rb+", col_sep: "\t", row_sep: "\r\n") do |csv|
      assert_equal(@expected.shift, csv.shift)
      assert_equal(@expected.shift, csv.shift)
      assert_equal(nil, csv.shift)
    end
  end
  
  def test_enumerators_are_supported
    CSV.open(@path, col_sep: "\t", row_sep: "\r\n") do |csv|
      enum = csv.each
      assert_instance_of(Enumerator, enum)
      assert_equal(@expected.shift, enum.next)
    end
  end

  ### Test Write Interface ###

  def test_generate
    str = CSV.generate do |csv|  # default empty String
      assert_instance_of(CSV, csv)
      assert_equal(csv, csv << [1, 2, 3])
      assert_equal(csv, csv << [4, nil, 5])
    end
    assert_not_nil(str)
    assert_instance_of(String, str)
    assert_equal("1,2,3\n4,,5\n", str)

    CSV.generate(str) do |csv|   # appending to a String
      assert_equal(csv, csv << ["last", %Q{"row"}])
    end
    assert_equal(%Q{1,2,3\n4,,5\nlast,"""row"""\n}, str)
  end

  def test_generate_line
    line = CSV.generate_line(%w{1 2 3}, col_sep: ";")
    assert_not_nil(line)
    assert_instance_of(String, line)
    assert_equal("1;2;3\n", line)

    # shortcut interface
    line = %w{1 2 3}.to_csv(col_sep: ";")
    assert_not_nil(line)
    assert_instance_of(String, line)
    assert_equal("1;2;3\n", line)
  end

  def test_write_header_detection
    File.unlink(@path)

    headers = %w{a b c}
    CSV.open(@path, "w", headers: true) do |csv|
      csv << headers
      csv << %w{1 2 3}
      assert_equal(headers, csv.instance_variable_get(:@headers))
    end
  end

  def test_write_lineno
    File.unlink(@path)

    CSV.open(@path, "w") do |csv|
      lines = 20
      lines.times { csv << %w{a b c} }
      assert_equal(lines, csv.lineno)
    end
  end

  def test_write_hash
    File.unlink(@path)

    lines = [{a: 1, b: 2, c: 3}, {a: 4, b: 5, c: 6}]
    CSV.open( @path, "wb", headers:           true,
                           header_converters: :symbol ) do |csv|
      csv << lines.first.keys
      lines.each { |line| csv << line }
    end
    CSV.open( @path, "rb", headers:           true,
                           converters:        :all,
                           header_converters: :symbol ) do |csv|
      csv.each { |line| assert_equal(lines.shift, line.to_hash) }
    end
  end

  def test_write_hash_with_headers_array
    File.unlink(@path)

    lines = [{a: 1, b: 2, c: 3}, {a: 4, b: 5, c: 6}]
    CSV.open(@path, "wb", headers: [:b, :a, :c]) do |csv|
      lines.each { |line| csv << line }
    end

    # test writing fields in the correct order
    File.open(@path, "rb") do |f|
      assert_equal("2,1,3", f.gets.strip)
      assert_equal("5,4,6", f.gets.strip)
    end

    # test reading CSV with headers
    CSV.open( @path, "rb", headers:    [:b, :a, :c],
                           converters: :all ) do |csv|
      csv.each { |line| assert_equal(lines.shift, line.to_hash) }
    end
  end

  def test_write_hash_with_headers_string
    File.unlink(@path)

    lines = [{"a" => 1, "b" => 2, "c" => 3}, {"a" => 4, "b" => 5, "c" => 6}]
    CSV.open(@path, "wb", headers: "b|a|c", col_sep: "|") do |csv|
      lines.each { |line| csv << line }
    end

    # test writing fields in the correct order
    File.open(@path, "rb") do |f|
      assert_equal("2|1|3", f.gets.strip)
      assert_equal("5|4|6", f.gets.strip)
    end

    # test reading CSV with headers
    CSV.open( @path, "rb", headers:    "b|a|c",
                           col_sep:    "|",
                           converters: :all ) do |csv|
      csv.each { |line| assert_equal(lines.shift, line.to_hash) }
    end
  end

  def test_write_headers
    File.unlink(@path)

    lines = [{"a" => 1, "b" => 2, "c" => 3}, {"a" => 4, "b" => 5, "c" => 6}]
    CSV.open( @path, "wb", headers:       "b|a|c",
                           write_headers: true,
                           col_sep:       "|" ) do |csv|
      lines.each { |line| csv << line }
    end

    # test writing fields in the correct order
    File.open(@path, "rb") do |f|
      assert_equal("b|a|c", f.gets.strip)
      assert_equal("2|1|3", f.gets.strip)
      assert_equal("5|4|6", f.gets.strip)
    end

    # test reading CSV with headers
    CSV.open( @path, "rb", headers:    true,
                           col_sep:    "|",
                           converters: :all ) do |csv|
      csv.each { |line| assert_equal(lines.shift, line.to_hash) }
    end
  end

  def test_append  # aliased add_row() and puts()
    File.unlink(@path)

    CSV.open(@path, "wb", col_sep: "\t", row_sep: "\r\n") do |csv|
      @expected.each { |row| csv << row }
    end

    test_shift

    # same thing using CSV::Row objects
    File.unlink(@path)

    CSV.open(@path, "wb", col_sep: "\t", row_sep: "\r\n") do |csv|
      @expected.each { |row| csv << CSV::Row.new(Array.new, row) }
    end

    test_shift
  end

  ### Test Read and Write Interface ###

  def test_filter
    assert_respond_to(CSV, :filter)

    expected = [[1, 2, 3], [4, 5]]
    CSV.filter( "1;2;3\n4;5\n", (result = String.new),
                in_col_sep: ";", out_col_sep: ",",
                converters: :all ) do |row|
      assert_equal(row, expected.shift)
      row.map! { |n| n * 2 }
      row << "Added\r"
    end
    assert_equal("2,4,6,\"Added\r\"\n8,10,\"Added\r\"\n", result)
  end

  def test_instance
    csv = String.new

    first = nil
    assert_nothing_raised(Exception) do
      first =  CSV.instance(csv, col_sep: ";")
      first << %w{a b c}
    end

    assert_equal("a;b;c\n", csv)

    second = nil
    assert_nothing_raised(Exception) do
      second =  CSV.instance(csv, col_sep: ";")
      second << [1, 2, 3]
    end

    assert_equal(first.object_id, second.object_id)
    assert_equal("a;b;c\n1;2;3\n", csv)

    # shortcuts
    assert_equal(STDOUT, CSV.instance.instance_eval { @io })
    assert_equal(STDOUT, CSV { |new_csv| new_csv.instance_eval { @io } })
  end

  def test_options_are_not_modified
    opt = {}.freeze
    assert_nothing_raised {  CSV.foreach(@path, opt)       }
    assert_nothing_raised {  CSV.open(@path, opt){}        }
    assert_nothing_raised {  CSV.parse("", opt)            }
    assert_nothing_raised {  CSV.parse_line("", opt)       }
    assert_nothing_raised {  CSV.read(@path, opt)          }
    assert_nothing_raised {  CSV.readlines(@path, opt)     }
    assert_nothing_raised {  CSV.table(@path, opt)         }
    assert_nothing_raised {  CSV.generate(opt){}           }
    assert_nothing_raised {  CSV.generate_line([], opt)    }
    assert_nothing_raised {  CSV.filter("", "", opt){}     }
    assert_nothing_raised {  CSV.instance("", opt)         }
  end
end
