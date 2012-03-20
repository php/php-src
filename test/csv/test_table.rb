#!/usr/bin/env ruby -w
# encoding: UTF-8

# tc_table.rb
#
#  Created by James Edward Gray II on 2005-10-31.
#  Copyright 2005 James Edward Gray II. You can redistribute or modify this code
#  under the terms of Ruby's license.

require_relative "base"

class TestCSV::Table < TestCSV
  extend DifferentOFS

  def setup
    super
    @rows  = [ CSV::Row.new(%w{A B C}, [1, 2, 3]),
               CSV::Row.new(%w{A B C}, [4, 5, 6]),
               CSV::Row.new(%w{A B C}, [7, 8, 9]) ]
    @table = CSV::Table.new(@rows)

    @header_table = CSV::Table.new(
      [CSV::Row.new(%w{A B C}, %w{A B C}, true)] + @rows
    )
  end

  def test_initialze
    assert_not_nil(@table)
    assert_instance_of(CSV::Table, @table)
  end

  def test_modes
    assert_equal(:col_or_row, @table.mode)

    # non-destructive changes, intended for one shot calls
    cols = @table.by_col
    assert_equal(:col_or_row, @table.mode)
    assert_equal(:col, cols.mode)
    assert_equal(@table, cols)

    rows = @table.by_row
    assert_equal(:col_or_row, @table.mode)
    assert_equal(:row, rows.mode)
    assert_equal(@table, rows)

    # destructive mode changing calls
    assert_equal(@table, @table.by_row!)
    assert_equal(:row, @table.mode)
    assert_equal(@table, @table.by_col_or_row!)
    assert_equal(:col_or_row, @table.mode)
  end

  def test_headers
    assert_equal(@rows.first.headers, @table.headers)
  end

  def test_index
    ##################
    ### Mixed Mode ###
    ##################
    # by row
    @rows.each_index { |i| assert_equal(@rows[i], @table[i]) }
    assert_equal(nil, @table[100])  # empty row

    # by col
    @rows.first.headers.each do |header|
      assert_equal(@rows.map { |row| row[header] }, @table[header])
    end
    assert_equal([nil] * @rows.size, @table["Z"])  # empty col

    # by cell, row then col
    assert_equal(2, @table[0][1])
    assert_equal(6, @table[1]["C"])

    # by cell, col then row
    assert_equal(5, @table["B"][1])
    assert_equal(9, @table["C"][2])

    # with headers (by col)
    assert_equal(["B", 2, 5, 8], @header_table["B"])

    ###################
    ### Column Mode ###
    ###################
    @table.by_col!

    assert_equal([2, 5, 8], @table[1])
    assert_equal([2, 5, 8], @table["B"])

    ################
    ### Row Mode ###
    ################
    @table.by_row!

    assert_equal(@rows[1], @table[1])
    assert_raise(TypeError) { @table["B"] }

    ############################
    ### One Shot Mode Change ###
    ############################
    assert_equal(@rows[1], @table[1])
    assert_equal([2, 5, 8], @table.by_col[1])
    assert_equal(@rows[1], @table[1])
  end

  def test_set_row_or_column
    ##################
    ### Mixed Mode ###
    ##################
    # set row
    @table[2] = [10, 11, 12]
    assert_equal([%w[A B C], [1, 2, 3], [4, 5, 6], [10, 11, 12]], @table.to_a)

    @table[3] = CSV::Row.new(%w[A B C], [13, 14, 15])
    assert_equal( [%w[A B C], [1, 2, 3], [4, 5, 6], [10, 11, 12], [13, 14, 15]],
                  @table.to_a )

    # set col
    @table["Type"] = "data"
    assert_equal( [ %w[A B C Type],
                    [1, 2, 3, "data"],
                    [4, 5, 6, "data"],
                    [10, 11, 12, "data"],
                    [13, 14, 15, "data"] ],
                  @table.to_a )

    @table["Index"] = [1, 2, 3]
    assert_equal( [ %w[A B C Type Index],
                    [1, 2, 3, "data", 1],
                    [4, 5, 6, "data", 2],
                    [10, 11, 12, "data", 3],
                    [13, 14, 15, "data", nil] ],
                  @table.to_a )

    @table["B"] = [100, 200]
    assert_equal( [ %w[A B C Type Index],
                    [1, 100, 3, "data", 1],
                    [4, 200, 6, "data", 2],
                    [10, nil, 12, "data", 3],
                    [13, nil, 15, "data", nil] ],
                  @table.to_a )

    # verify resulting table
    assert_equal(<<-END_RESULT.gsub(/^\s+/, ""), @table.to_csv)
    A,B,C,Type,Index
    1,100,3,data,1
    4,200,6,data,2
    10,,12,data,3
    13,,15,data,
    END_RESULT

    # with headers
    @header_table["Type"] = "data"
    assert_equal(%w[Type data data data], @header_table["Type"])

    ###################
    ### Column Mode ###
    ###################
    @table.by_col!

    @table[1] = [2, 5, 11, 14]
    assert_equal( [ %w[A B C Type Index],
                    [1, 2, 3, "data", 1],
                    [4, 5, 6, "data", 2],
                    [10, 11, 12, "data", 3],
                    [13, 14, 15, "data", nil] ],
                  @table.to_a )

    @table["Extra"] = "new stuff"
    assert_equal( [ %w[A B C Type Index Extra],
                    [1, 2, 3, "data", 1, "new stuff"],
                    [4, 5, 6, "data", 2, "new stuff"],
                    [10, 11, 12, "data", 3, "new stuff"],
                    [13, 14, 15, "data", nil, "new stuff"] ],
                  @table.to_a )

    ################
    ### Row Mode ###
    ################
    @table.by_row!

    @table[1] = (1..6).to_a
    assert_equal( [ %w[A B C Type Index Extra],
                    [1, 2, 3, "data", 1, "new stuff"],
                    [1, 2, 3, 4, 5, 6],
                    [10, 11, 12, "data", 3, "new stuff"],
                    [13, 14, 15, "data", nil, "new stuff"] ],
                  @table.to_a )

    assert_raise(TypeError) { @table["Extra"] = nil }
  end

  def test_each
    ######################
    ### Mixed/Row Mode ###
    ######################
    i = 0
    @table.each do |row|
      assert_equal(@rows[i], row)
      i += 1
    end

    # verify that we can chain the call
    assert_equal(@table, @table.each { })

    ###################
    ### Column Mode ###
    ###################
    @table.by_col!

    headers = @table.headers
    @table.each do |header, column|
      assert_equal(headers.shift, header)
      assert_equal(@table[header], column)
    end

    ############################
    ### One Shot Mode Change ###
    ############################
    @table.by_col_or_row!

    @table.each { |row| assert_instance_of(CSV::Row, row) }
    @table.by_col.each { |tuple| assert_instance_of(Array, tuple) }
    @table.each { |row| assert_instance_of(CSV::Row, row) }
  end

  def test_enumerable
    assert_equal( @rows.values_at(0, 2),
                  @table.select { |row| (row["B"] % 2).zero? } )

    assert_equal(@rows[1], @table.find { |row| row["C"] > 5 })
  end

  def test_to_a
    assert_equal([%w[A B C], [1, 2, 3], [4, 5, 6], [7, 8, 9]], @table.to_a)

    # with headers
    assert_equal( [%w[A B C], [1, 2, 3], [4, 5, 6], [7, 8, 9]],
                  @header_table.to_a )
  end

  def test_to_csv
    csv = <<-END_CSV.gsub(/^\s+/, "")
    A,B,C
    1,2,3
    4,5,6
    7,8,9
    END_CSV

    # normal conversion
    assert_equal(csv, @table.to_csv)
    assert_equal(csv, @table.to_s)  # alias

    # with options
    assert_equal( csv.gsub(",", "|").gsub("\n", "\r\n"),
                  @table.to_csv(col_sep: "|", row_sep: "\r\n") )
    assert_equal( csv.lines.to_a[1..-1].join(''),
                  @table.to_csv(:write_headers => false) )

    # with headers
    assert_equal(csv, @header_table.to_csv)
  end

  def test_append
    # verify that we can chain the call
    assert_equal(@table, @table << [10, 11, 12])

    # Array append
    assert_equal(CSV::Row.new(%w[A B C], [10, 11, 12]), @table[-1])

    # Row append
    assert_equal(@table, @table << CSV::Row.new(%w[A B C], [13, 14, 15]))
    assert_equal(CSV::Row.new(%w[A B C], [13, 14, 15]), @table[-1])
  end

  def test_delete_mixed
    ##################
    ### Mixed Mode ###
    ##################
    # delete a row
    assert_equal(@rows[1], @table.delete(1))

    # delete a col
    assert_equal(@rows.map { |row| row["A"] }, @table.delete("A"))

    # verify resulting table
    assert_equal(<<-END_RESULT.gsub(/^\s+/, ""), @table.to_csv)
    B,C
    2,3
    8,9
    END_RESULT
  end

  def test_delete_column
    ###################
    ### Column Mode ###
    ###################
    @table.by_col!

    assert_equal(@rows.map { |row| row[0] }, @table.delete(0))
    assert_equal(@rows.map { |row| row["C"] }, @table.delete("C"))

    # verify resulting table
    assert_equal(<<-END_RESULT.gsub(/^\s+/, ""), @table.to_csv)
    B
    2
    5
    8
    END_RESULT
  end

  def test_delete_row
    ################
    ### Row Mode ###
    ################
    @table.by_row!

    assert_equal(@rows[1], @table.delete(1))
    assert_raise(TypeError) { @table.delete("C") }

    # verify resulting table
    assert_equal(<<-END_RESULT.gsub(/^\s+/, ""), @table.to_csv)
    A,B,C
    1,2,3
    7,8,9
    END_RESULT
  end

  def test_delete_with_blank_rows
    data = "col1,col2\nra1,ra2\n\nrb1,rb2"
    table = CSV.parse(data, :headers => true)
    assert_equal(["ra2", nil, "rb2"], table.delete("col2"))
  end

  def test_delete_if_row
    ######################
    ### Mixed/Row Mode ###
    ######################
    # verify that we can chain the call
    assert_equal(@table, @table.delete_if { |row| (row["B"] % 2).zero? })

    # verify resulting table
    assert_equal(<<-END_RESULT.gsub(/^\s+/, ""), @table.to_csv)
    A,B,C
    4,5,6
    END_RESULT
  end

  def test_delete_if_column
    ###################
    ### Column Mode ###
    ###################
    @table.by_col!

    assert_equal(@table, @table.delete_if { |h, v| h > "A" })
    assert_equal(<<-END_RESULT.gsub(/^\s+/, ""), @table.to_csv)
    A
    1
    4
    7
    END_RESULT
  end

  def test_values_at
    ##################
    ### Mixed Mode ###
    ##################
    # rows
    assert_equal(@rows.values_at(0, 2), @table.values_at(0, 2))
    assert_equal(@rows.values_at(1..2), @table.values_at(1..2))

    # cols
    assert_equal([[1, 3], [4, 6], [7, 9]], @table.values_at("A", "C"))
    assert_equal([[2, 3], [5, 6], [8, 9]], @table.values_at("B".."C"))

    ###################
    ### Column Mode ###
    ###################
    @table.by_col!

    assert_equal([[1, 3], [4, 6], [7, 9]], @table.values_at(0, 2))
    assert_equal([[1, 3], [4, 6], [7, 9]], @table.values_at("A", "C"))

    ################
    ### Row Mode ###
    ################
    @table.by_row!

    assert_equal(@rows.values_at(0, 2), @table.values_at(0, 2))
    assert_raise(TypeError) { @table.values_at("A", "C") }

    ############################
    ### One Shot Mode Change ###
    ############################
    assert_equal(@rows.values_at(0, 2), @table.values_at(0, 2))
    assert_equal([[1, 3], [4, 6], [7, 9]], @table.by_col.values_at(0, 2))
    assert_equal(@rows.values_at(0, 2), @table.values_at(0, 2))
  end

  def test_array_delegation
    assert(!@table.empty?, "Table was empty.")

    assert_equal(@rows.size, @table.size)
  end

  def test_inspect_shows_current_mode
    str = @table.inspect
    assert(str.include?("mode:#{@table.mode}"), "Mode not shown.")

    @table.by_col!
    str = @table.inspect
    assert(str.include?("mode:#{@table.mode}"), "Mode not shown.")
  end

  def test_inspect_encoding_is_ascii_compatible
    assert( Encoding.compatible?( Encoding.find("US-ASCII"),
                                  @table.inspect.encoding ),
            "inspect() was not ASCII compatible." )
  end
end
