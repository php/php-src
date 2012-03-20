begin
  require 'ripper'
  require 'test/unit'
  ripper_test = true
  module TestRipper; end
rescue LoadError
end

class TestRipper::Filter < Test::Unit::TestCase

  class Filter < Ripper::Filter
    def on_default(event, token, data)
      if data.empty?
        data[:filename] = filename rescue nil
        data[:lineno] = lineno
        data[:column] = column
        data[:token] = token
      end
      data
    end
  end

  def filename
    File.expand_path(__FILE__)
  end

  def test_filter_filename_unset
    data = {}
    filter = Filter.new(File.read(filename))
    filter.parse(data)
    assert_equal('-', data[:filename], "[ruby-dev:37856]")
    assert_equal('-', filter.filename)
  end

  def test_filter_filename
    data = {}
    filter = Filter.new(File.read(filename), filename)
    assert_equal(filename, filter.filename)
    filter.parse(data)
    assert_equal(filename, data[:filename])
    assert_equal(filename, filter.filename)
  end

  def test_filter_lineno
    data = {}
    src = File.read(filename)
    src_lines = src.count("\n")
    filter = Filter.new(src)
    assert_equal(nil, filter.lineno)
    filter.parse(data)
    assert_equal(1, data[:lineno])
    assert_equal(src_lines, filter.lineno)
  end

  def test_filter_lineno_set
    data = {}
    src = File.read(filename)
    src_lines = src.count("\n")
    filter = Filter.new(src, '-', 100)
    assert_equal(nil, filter.lineno)
    filter.parse(data)
    assert_equal(100, data[:lineno])
    assert_equal(src_lines+100-1, filter.lineno)
  end

  def test_filter_column
    data = {}
    src = File.read(filename)
    last_columns = src[/(.*)\Z/].size
    filter = Filter.new(src)
    assert_equal(nil, filter.column)
    filter.parse(data)
    assert_equal(0, data[:column])
    assert_equal(last_columns, filter.column)
  end

  def test_filter_token
    data = {}
    filter = Filter.new(File.read(filename))
    filter.parse(data)
    assert_equal("begin", data[:token])
  end
end if ripper_test
