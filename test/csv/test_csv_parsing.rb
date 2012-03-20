#!/usr/bin/env ruby -w
# encoding: UTF-8

# tc_csv_parsing.rb
#
#  Created by James Edward Gray II on 2005-10-31.
#  Copyright 2005 James Edward Gray II. You can redistribute or modify this code
#  under the terms of Ruby's license.

require "timeout"

require_relative "base"

#
# Following tests are my interpretation of the
# {CSV RCF}[http://www.ietf.org/rfc/rfc4180.txt].  I only deviate from that
# document in one place (intentionally) and that is to make the default row
# separator <tt>$/</tt>.
#
class TestCSV::Parsing < TestCSV
  extend DifferentOFS

  BIG_DATA = "123456789\n" * 1024

  def test_mastering_regex_example
    ex = %Q{Ten Thousand,10000, 2710 ,,"10,000","It's ""10 Grand"", baby",10K}
    assert_equal( [ "Ten Thousand", "10000", " 2710 ", nil, "10,000",
                    "It's \"10 Grand\", baby", "10K" ],
                  CSV.parse_line(ex) )
  end

  # Old Ruby 1.8 CSV library tests.
  def test_std_lib_csv
    [ ["\t", ["\t"]],
      ["foo,\"\"\"\"\"\",baz", ["foo", "\"\"", "baz"]],
      ["foo,\"\"\"bar\"\"\",baz", ["foo", "\"bar\"", "baz"]],
      ["\"\"\"\n\",\"\"\"\n\"", ["\"\n", "\"\n"]],
      ["foo,\"\r\n\",baz", ["foo", "\r\n", "baz"]],
      ["\"\"", [""]],
      ["foo,\"\"\"\",baz", ["foo", "\"", "baz"]],
      ["foo,\"\r.\n\",baz", ["foo", "\r.\n", "baz"]],
      ["foo,\"\r\",baz", ["foo", "\r", "baz"]],
      ["foo,\"\",baz", ["foo", "", "baz"]],
      ["\",\"", [","]],
      ["foo", ["foo"]],
      [",,", [nil, nil, nil]],
      [",", [nil, nil]],
      ["foo,\"\n\",baz", ["foo", "\n", "baz"]],
      ["foo,,baz", ["foo", nil, "baz"]],
      ["\"\"\"\r\",\"\"\"\r\"", ["\"\r", "\"\r"]],
      ["\",\",\",\"", [",", ","]],
      ["foo,bar,", ["foo", "bar", nil]],
      [",foo,bar", [nil, "foo", "bar"]],
      ["foo,bar", ["foo", "bar"]],
      [";", [";"]],
      ["\t,\t", ["\t", "\t"]],
      ["foo,\"\r\n\r\",baz", ["foo", "\r\n\r", "baz"]],
      ["foo,\"\r\n\n\",baz", ["foo", "\r\n\n", "baz"]],
      ["foo,\"foo,bar\",baz", ["foo", "foo,bar", "baz"]],
      [";,;", [";", ";"]] ].each do |csv_test|
      assert_equal(csv_test.last, CSV.parse_line(csv_test.first))
    end

    [ ["foo,\"\"\"\"\"\",baz", ["foo", "\"\"", "baz"]],
      ["foo,\"\"\"bar\"\"\",baz", ["foo", "\"bar\"", "baz"]],
      ["foo,\"\r\n\",baz", ["foo", "\r\n", "baz"]],
      ["\"\"", [""]],
      ["foo,\"\"\"\",baz", ["foo", "\"", "baz"]],
      ["foo,\"\r.\n\",baz", ["foo", "\r.\n", "baz"]],
      ["foo,\"\r\",baz", ["foo", "\r", "baz"]],
      ["foo,\"\",baz", ["foo", "", "baz"]],
      ["foo", ["foo"]],
      [",,", [nil, nil, nil]],
      [",", [nil, nil]],
      ["foo,\"\n\",baz", ["foo", "\n", "baz"]],
      ["foo,,baz", ["foo", nil, "baz"]],
      ["foo,bar", ["foo", "bar"]],
      ["foo,\"\r\n\n\",baz", ["foo", "\r\n\n", "baz"]],
      ["foo,\"foo,bar\",baz", ["foo", "foo,bar", "baz"]] ].each do |csv_test|
      assert_equal(csv_test.last, CSV.parse_line(csv_test.first))
    end
  end

  # From:  http://ruby-talk.org/cgi-bin/scat.rb/ruby/ruby-core/6496
  def test_aras_edge_cases
    [ [%Q{a,b},               ["a", "b"]],
      [%Q{a,"""b"""},         ["a", "\"b\""]],
      [%Q{a,"""b"},           ["a", "\"b"]],
      [%Q{a,"b"""},           ["a", "b\""]],
      [%Q{a,"\nb"""},         ["a", "\nb\""]],
      [%Q{a,"""\nb"},         ["a", "\"\nb"]],
      [%Q{a,"""\nb\n"""},     ["a", "\"\nb\n\""]],
      [%Q{a,"""\nb\n""",\nc}, ["a", "\"\nb\n\"", nil]],
      [%Q{a,,,},              ["a", nil, nil, nil]],
      [%Q{,},                 [nil, nil]],
      [%Q{"",""},             ["", ""]],
      [%Q{""""},              ["\""]],
      [%Q{"""",""},           ["\"",""]],
      [%Q{,""},               [nil,""]],
      [%Q{,"\r"},             [nil,"\r"]],
      [%Q{"\r\n,"},           ["\r\n,"]],
      [%Q{"\r\n,",},          ["\r\n,", nil]] ].each do |edge_case|
        assert_equal(edge_case.last, CSV.parse_line(edge_case.first))
      end
  end

  def test_james_edge_cases
    # A read at eof? should return nil.
    assert_equal(nil, CSV.parse_line(""))
    #
    # With Ruby 1.8 CSV it's impossible to tell an empty line from a line
    # containing a single +nil+ field.  The old CSV library returns
    # <tt>[nil]</tt> in these cases, but <tt>Array.new</tt> makes more sense to
    # me.
    #
    assert_equal(Array.new, CSV.parse_line("\n1,2,3\n"))
  end

  def test_rob_edge_cases
    [ [%Q{"a\nb"},                         ["a\nb"]],
      [%Q{"\n\n\n"},                       ["\n\n\n"]],
      [%Q{a,"b\n\nc"},                     ['a', "b\n\nc"]],
      [%Q{,"\r\n"},                        [nil,"\r\n"]],
      [%Q{,"\r\n."},                       [nil,"\r\n."]],
      [%Q{"a\na","one newline"},           ["a\na", 'one newline']],
      [%Q{"a\n\na","two newlines"},        ["a\n\na", 'two newlines']],
      [%Q{"a\r\na","one CRLF"},            ["a\r\na", 'one CRLF']],
      [%Q{"a\r\n\r\na","two CRLFs"},       ["a\r\n\r\na", 'two CRLFs']],
      [%Q{with blank,"start\n\nfinish"\n}, ['with blank', "start\n\nfinish"]],
    ].each do |edge_case|
      assert_equal(edge_case.last, CSV.parse_line(edge_case.first))
    end
  end

  def test_non_regex_edge_cases
    # An early version of the non-regex parser fails this test
    [ [ "foo,\"foo,bar,baz,foo\",\"foo\"",
        ["foo", "foo,bar,baz,foo", "foo"] ] ].each do |edge_case|
      assert_equal(edge_case.last, CSV.parse_line(edge_case.first))
    end

    assert_raise(CSV::MalformedCSVError) do
      CSV.parse_line("1,\"23\"4\"5\", 6")
    end
  end

  def test_malformed_csv
    assert_raise(CSV::MalformedCSVError) do
      CSV.parse_line("1,2\r,3", row_sep: "\n")
    end

    bad_data = <<-END_DATA.gsub(/^ +/, "")
    line,1,abc
    line,2,"def\nghi"

    line,4,some\rjunk
    line,5,jkl
    END_DATA
    lines = bad_data.lines.to_a
    assert_equal(6, lines.size)
    assert_match(/\Aline,4/, lines.find { |l| l =~ /some\rjunk/ })

    csv = CSV.new(bad_data)
    begin
      loop do
        assert_not_nil(csv.shift)
        assert_send([csv.lineno, :<, 4])
      end
    rescue CSV::MalformedCSVError
      assert_equal( "Unquoted fields do not allow \\r or \\n (line 4).",
                    $!.message )
    end

    assert_raise(CSV::MalformedCSVError) { CSV.parse_line('1,2,"3...') }

    bad_data = <<-END_DATA.gsub(/^ +/, "")
    line,1,abc
    line,2,"def\nghi"

    line,4,8'10"
    line,5,jkl
    END_DATA
    lines = bad_data.lines.to_a
    assert_equal(6, lines.size)
    assert_match(/\Aline,4/, lines.find { |l| l =~ /8'10"/ })

    csv = CSV.new(bad_data)
    begin
      loop do
        assert_not_nil(csv.shift)
        assert_send([csv.lineno, :<, 4])
      end
    rescue CSV::MalformedCSVError
      assert_equal("Illegal quoting in line 4.", $!.message)
    end
  end

  def test_the_parse_fails_fast_when_it_can_for_unquoted_fields
    assert_parse_errors_out('valid,fields,bad start"' + BIG_DATA)
  end

  def test_the_parse_fails_fast_when_it_can_for_unescaped_quotes
    assert_parse_errors_out('valid,fields,"bad start"unescaped' + BIG_DATA)
  end

  def test_field_size_limit_controls_lookahead
    assert_parse_errors_out( 'valid,fields,"' + BIG_DATA + '"',
                             field_size_limit: 2048 )
  end

  private

  def assert_parse_errors_out(*args)
    assert_raise(CSV::MalformedCSVError) do
      Timeout.timeout(0.2) do
        CSV.parse(*args)
        fail("Parse didn't error out")
      end
    end
  end
end
