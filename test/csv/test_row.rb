#!/usr/bin/env ruby -w
# encoding: UTF-8

# tc_row.rb
#
#  Created by James Edward Gray II on 2005-10-31.
#  Copyright 2005 James Edward Gray II. You can redistribute or modify this code
#  under the terms of Ruby's license.

require_relative "base"

class TestCSV::Row < TestCSV
  extend DifferentOFS

  def setup
    super
    @row = CSV::Row.new(%w{A B C A A}, [1, 2, 3, 4])
  end

  def test_initialize
    # basic
    row = CSV::Row.new(%w{A B C}, [1, 2, 3])
    assert_not_nil(row)
    assert_instance_of(CSV::Row, row)
    assert_equal([["A", 1], ["B", 2], ["C", 3]], row.to_a)

    # missing headers
    row = CSV::Row.new(%w{A}, [1, 2, 3])
    assert_not_nil(row)
    assert_instance_of(CSV::Row, row)
    assert_equal([["A", 1], [nil, 2], [nil, 3]], row.to_a)

    # missing fields
    row = CSV::Row.new(%w{A B C}, [1, 2])
    assert_not_nil(row)
    assert_instance_of(CSV::Row, row)
    assert_equal([["A", 1], ["B", 2], ["C", nil]], row.to_a)
  end

  def test_row_type
    # field rows
    row = CSV::Row.new(%w{A B C}, [1, 2, 3])         # implicit
    assert(!row.header_row?)
    assert(row.field_row?)
    row = CSV::Row.new(%w{A B C}, [1, 2, 3], false)  # explicit
    assert(!row.header_row?)
    assert(row.field_row?)

    # header row
    row = CSV::Row.new(%w{A B C}, [1, 2, 3], true)
    assert(row.header_row?)
    assert(!row.field_row?)
  end

  def test_headers
    assert_equal(%w{A B C A A}, @row.headers)
  end

  def test_field
    # by name
    assert_equal(2, @row.field("B"))
    assert_equal(2, @row["B"])  # alias

    # by index
    assert_equal(3, @row.field(2))

    # missing
    assert_nil(@row.field("Missing"))
    assert_nil(@row.field(10))

    # minimum index
    assert_equal(1, @row.field("A"))
    assert_equal(1, @row.field("A", 0))
    assert_equal(4, @row.field("A", 1))
    assert_equal(4, @row.field("A", 2))
    assert_equal(4, @row.field("A", 3))
    assert_equal(nil, @row.field("A", 4))
    assert_equal(nil, @row.field("A", 5))
  end

  def test_set_field
    # set field by name
    assert_equal(100, @row["A"] = 100)

    # set field by index
    assert_equal(300, @row[3] = 300)

    # set field by name and minimum index
    assert_equal([:a, :b, :c], @row["A", 4] = [:a, :b, :c])

    # verify the changes
    assert_equal( [ ["A", 100],
                    ["B", 2],
                    ["C", 3],
                    ["A", 300],
                    ["A", [:a, :b, :c]] ], @row.to_a )

    # assigning an index past the end
    assert_equal("End", @row[10] = "End")
    assert_equal( [ ["A", 100],
                    ["B", 2],
                    ["C", 3],
                    ["A", 300],
                    ["A", [:a, :b, :c]],
                    [nil, nil],
                    [nil, nil],
                    [nil, nil],
                    [nil, nil],
                    [nil, nil],
                    [nil, "End"] ], @row.to_a )

    # assigning a new field by header
    assert_equal("New", @row[:new] = "New")
    assert_equal( [ ["A", 100],
                    ["B", 2],
                    ["C", 3],
                    ["A", 300],
                    ["A", [:a, :b, :c]],
                    [nil, nil],
                    [nil, nil],
                    [nil, nil],
                    [nil, nil],
                    [nil, nil],
                    [nil, "End"],
                    [:new, "New"] ], @row.to_a )
  end

  def test_append
    # add a value
    assert_equal(@row, @row << "Value")
    assert_equal( [ ["A", 1],
                    ["B", 2],
                    ["C", 3],
                    ["A", 4],
                    ["A", nil],
                    [nil, "Value"] ], @row.to_a )

    # add a pair
    assert_equal(@row, @row << %w{Header Field})
    assert_equal( [ ["A", 1],
                    ["B", 2],
                    ["C", 3],
                    ["A", 4],
                    ["A", nil],
                    [nil, "Value"],
                    %w{Header Field} ], @row.to_a )

    # a pair with Hash syntax
    assert_equal(@row, @row << {key: :value})
    assert_equal( [ ["A", 1],
                    ["B", 2],
                    ["C", 3],
                    ["A", 4],
                    ["A", nil],
                    [nil, "Value"],
                    %w{Header Field},
                    [:key, :value] ], @row.to_a )

    # multiple fields at once
    assert_equal(@row, @row.push(100, 200, [:last, 300]))
    assert_equal( [ ["A", 1],
                    ["B", 2],
                    ["C", 3],
                    ["A", 4],
                    ["A", nil],
                    [nil, "Value"],
                    %w{Header Field},
                    [:key, :value],
                    [nil, 100],
                    [nil, 200],
                    [:last, 300] ], @row.to_a )
  end

  def test_delete
    # by index
    assert_equal(["B", 2], @row.delete(1))

    # by header
    assert_equal(["C", 3], @row.delete("C"))

    # using a block
    assert_equal(@row, @row.delete_if { |h, f| h == "A" and not f.nil? })
    assert_equal([["A", nil]], @row.to_a)
  end

  def test_fields
    # all fields
    assert_equal([1, 2, 3, 4, nil], @row.fields)

    # by header
    assert_equal([1, 3], @row.fields("A", "C"))

    # by index
    assert_equal([2, 3, nil], @row.fields(1, 2, 10))

    # by both
    assert_equal([2, 3, 4], @row.fields("B", "C", 3))

    # with minimum indices
    assert_equal([2, 3, 4], @row.fields("B", "C", ["A", 3]))

    # by header range
    assert_equal([2, 3], @row.values_at("B".."C"))
  end

  def test_index
    # basic usage
    assert_equal(0, @row.index("A"))
    assert_equal(1, @row.index("B"))
    assert_equal(2, @row.index("C"))
    assert_equal(nil, @row.index("Z"))

    # with minimum index
    assert_equal(0, @row.index("A"))
    assert_equal(0, @row.index("A", 0))
    assert_equal(3, @row.index("A", 1))
    assert_equal(3, @row.index("A", 2))
    assert_equal(3, @row.index("A", 3))
    assert_equal(4, @row.index("A", 4))
    assert_equal(nil, @row.index("A", 5))
  end

  def test_queries
    # headers
    assert(@row.header?("A"))
    assert(@row.header?("C"))
    assert(!@row.header?("Z"))
    assert(@row.include?("A"))  # alias

    # fields
    assert(@row.field?(4))
    assert(@row.field?(nil))
    assert(!@row.field?(10))
  end

  def test_each
    # array style
    ary = @row.to_a
    @row.each do |pair|
      assert_equal(ary.first.first, pair.first)
      assert_equal(ary.shift.last, pair.last)
    end

    # hash style
    ary = @row.to_a
    @row.each do |header, field|
      assert_equal(ary.first.first, header)
      assert_equal(ary.shift.last, field)
    end

    # verify that we can chain the call
    assert_equal(@row, @row.each { })
  end

  def test_enumerable
    assert_equal( [["A", 1], ["A", 4], ["A", nil]],
                  @row.select { |pair| pair.first == "A" } )

    assert_equal(10, @row.inject(0) { |sum, (header, n)| sum + (n || 0) })
  end

  def test_to_a
    row = CSV::Row.new(%w{A B C}, [1, 2, 3]).to_a
    assert_instance_of(Array, row)
    row.each do |pair|
      assert_instance_of(Array, pair)
      assert_equal(2, pair.size)
    end
    assert_equal([["A", 1], ["B", 2], ["C", 3]], row)
  end

  def test_to_hash
    assert_equal({"A" => nil, "B" => 2, "C" => 3}, @row.to_hash)
  end

  def test_to_csv
    # normal conversion
    assert_equal("1,2,3,4,\n", @row.to_csv)
    assert_equal("1,2,3,4,\n", @row.to_s)  # alias

    # with options
    assert_equal( "1|2|3|4|\r\n",
                  @row.to_csv(col_sep: "|", row_sep: "\r\n") )
  end

  def test_array_delegation
    assert(!@row.empty?, "Row was empty.")

    assert_equal([@row.headers.size, @row.fields.size].max, @row.size)
  end

  def test_inspect_shows_header_field_pairs
    str = @row.inspect
    @row.each do |header, field|
      assert( str.include?("#{header.inspect}:#{field.inspect}"),
              "Header field pair not found." )
    end
  end

  def test_inspect_encoding_is_ascii_compatible
    assert( Encoding.compatible?( Encoding.find("US-ASCII"),
                                  @row.inspect.encoding ),
            "inspect() was not ASCII compatible." )
  end

  def test_inspect_shows_symbol_headers_as_bare_attributes
    str = CSV::Row.new(@row.headers.map { |h| h.to_sym }, @row.fields).inspect
    @row.each do |header, field|
      assert( str.include?("#{header}:#{field.inspect}"),
              "Header field pair not found." )
    end
  end
end
