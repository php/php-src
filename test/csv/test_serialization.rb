#!/usr/bin/env ruby -w
# encoding: UTF-8

# tc_serialization.rb
#
#  Created by James Edward Gray II on 2005-10-31.
#  Copyright 2005 James Edward Gray II. You can redistribute or modify this code
#  under the terms of Ruby's license.

require_relative "base"

# An example of how to provide custom CSV serialization.
class Hash
  def self.csv_load( meta, headers, fields )
    self[*headers.zip(fields).to_a.flatten.map { |e| eval(e) }]
  end

  def csv_headers
    keys.map { |key| key.inspect }
  end

  def csv_dump( headers )
    headers.map { |header| fetch(eval(header)).inspect }
  end
end

class TestCSV::Serialization < TestCSV
  extend DifferentOFS

  ### Classes Used to Test Serialization ###

  class ReadOnlyName
    def initialize( first, last )
      @first, @last = first, last
    end

    attr_reader :first, :last

    def ==( other )
      %w{first last}.all? { |att| send(att) == other.send(att) }
    end
  end

  Name = Struct.new(:first, :last)

  class FullName < Name
    def initialize( first, last, suffix = nil )
      super(first, last)

      @suffix = suffix
    end

    attr_accessor :suffix

    def ==( other )
      %w{first last suffix}.all? { |att| send(att) == other.send(att) }
    end
  end

  ### Tests ###

  def test_class_dump
    @names = [ %w{James Gray},
              %w{Dana Gray},
              %w{Greg Brown} ].map do |first, last|
      ReadOnlyName.new(first, last)
    end

    assert_nothing_raised(Exception) do
      @data = CSV.dump(@names)
    end
    assert_equal(<<-END_CLASS_DUMP.gsub(/^\s*/, ""), @data)
    class,TestCSV::Serialization::ReadOnlyName
    @first,@last
    James,Gray
    Dana,Gray
    Greg,Brown
    END_CLASS_DUMP
  end

  def test_struct_dump
    @names = [ %w{James Gray},
              %w{Dana Gray},
              %w{Greg Brown} ].map do |first, last|
      Name.new(first, last)
    end

    assert_nothing_raised(Exception) do
      @data = CSV.dump(@names)
    end
    assert_equal(<<-END_STRUCT_DUMP.gsub(/^\s*/, ""), @data)
    class,TestCSV::Serialization::Name
    first=,last=
    James,Gray
    Dana,Gray
    Greg,Brown
    END_STRUCT_DUMP
  end

  def test_inherited_struct_dump
    @names = [ %w{James Gray II},
              %w{Dana Gray},
              %w{Greg Brown} ].map do |first, last, suffix|
      FullName.new(first, last, suffix)
    end

    assert_nothing_raised(Exception) do
      @data = CSV.dump(@names)
    end
    assert_equal(<<-END_STRUCT_DUMP.gsub(/^\s*/, ""), @data)
    class,TestCSV::Serialization::FullName
    @suffix,first=,last=
    II,James,Gray
    ,Dana,Gray
    ,Greg,Brown
    END_STRUCT_DUMP
  end

  def test_load
    %w{ test_class_dump
        test_struct_dump
        test_inherited_struct_dump }.each do |test|
      send(test)
      CSV.load(@data).each do |loaded|
        assert_instance_of(@names.first.class, loaded)
        assert_equal(@names.shift, loaded)
      end
    end
  end

  def test_io
    test_class_dump

    data_file = File.join(File.dirname(__FILE__), "serialization_test_data.csv")
    CSV.dump(@names, File.open(data_file, "wb"))

    assert(File.exist?(data_file))
    assert_equal(<<-END_IO_DUMP.gsub(/^\s*/, ""), File.read(data_file))
    class,TestCSV::Serialization::ReadOnlyName
    @first,@last
    James,Gray
    Dana,Gray
    Greg,Brown
    END_IO_DUMP

    assert_equal(@names, CSV.load(File.open(data_file)))

    File.unlink(data_file)
  end

  def test_custom_dump_and_load
    obj = {1 => "simple", test: Hash}
    assert_equal(obj, CSV.load(CSV.dump([obj])).first)
  end
end
