# encoding: US-ASCII
# = csv.rb -- CSV Reading and Writing
#
#  Created by James Edward Gray II on 2005-10-31.
#  Copyright 2005 James Edward Gray II. You can redistribute or modify this code
#  under the terms of Ruby's license.
#
# See CSV for documentation.
#
# == Description
#
# Welcome to the new and improved CSV.
#
# This version of the CSV library began its life as FasterCSV.  FasterCSV was
# intended as a replacement to Ruby's then standard CSV library.  It was
# designed to address concerns users of that library had and it had three
# primary goals:
#
# 1.  Be significantly faster than CSV while remaining a pure Ruby library.
# 2.  Use a smaller and easier to maintain code base.  (FasterCSV eventually
#     grew larger, was also but considerably richer in features.  The parsing
#     core remains quite small.)
# 3.  Improve on the CSV interface.
#
# Obviously, the last one is subjective.  I did try to defer to the original
# interface whenever I didn't have a compelling reason to change it though, so
# hopefully this won't be too radically different.
#
# We must have met our goals because FasterCSV was renamed to CSV and replaced
# the original library as of Ruby 1.9. If you are migrating code from 1.8 or
# earlier, you may have to change your code to comply with the new interface.
#
# == What's Different From the Old CSV?
#
# I'm sure I'll miss something, but I'll try to mention most of the major
# differences I am aware of, to help others quickly get up to speed:
#
# === CSV Parsing
#
# * This parser is m17n aware.  See CSV for full details.
# * This library has a stricter parser and will throw MalformedCSVErrors on
#   problematic data.
# * This library has a less liberal idea of a line ending than CSV.  What you
#   set as the <tt>:row_sep</tt> is law.  It can auto-detect your line endings
#   though.
# * The old library returned empty lines as <tt>[nil]</tt>.  This library calls
#   them <tt>[]</tt>.
# * This library has a much faster parser.
#
# === Interface
#
# * CSV now uses Hash-style parameters to set options.
# * CSV no longer has generate_row() or parse_row().
# * The old CSV's Reader and Writer classes have been dropped.
# * CSV::open() is now more like Ruby's open().
# * CSV objects now support most standard IO methods.
# * CSV now has a new() method used to wrap objects like String and IO for
#   reading and writing.
# * CSV::generate() is different from the old method.
# * CSV no longer supports partial reads.  It works line-by-line.
# * CSV no longer allows the instance methods to override the separators for
#   performance reasons.  They must be set in the constructor.
#
# If you use this library and find yourself missing any functionality I have
# trimmed, please {let me know}[mailto:james@grayproductions.net].
#
# == Documentation
#
# See CSV for documentation.
#
# == What is CSV, really?
#
# CSV maintains a pretty strict definition of CSV taken directly from
# {the RFC}[http://www.ietf.org/rfc/rfc4180.txt].  I relax the rules in only one
# place and that is to make using this library easier.  CSV will parse all valid
# CSV.
#
# What you don't want to do is feed CSV invalid data.  Because of the way the
# CSV format works, it's common for a parser to need to read until the end of
# the file to be sure a field is invalid.  This eats a lot of time and memory.
#
# Luckily, when working with invalid CSV, Ruby's built-in methods will almost
# always be superior in every way.  For example, parsing non-quoted fields is as
# easy as:
#
#   data.split(",")
#
# == Questions and/or Comments
#
# Feel free to email {James Edward Gray II}[mailto:james@grayproductions.net]
# with any questions.

require "forwardable"
require "English"
require "date"
require "stringio"

#
# This class provides a complete interface to CSV files and data.  It offers
# tools to enable you to read and write to and from Strings or IO objects, as
# needed.
#
# == Reading
#
# === From a File
#
# ==== A Line at a Time
#
#   CSV.foreach("path/to/file.csv") do |row|
#     # use row here...
#   end
#
# ==== All at Once
#
#   arr_of_arrs = CSV.read("path/to/file.csv")
#
# === From a String
#
# ==== A Line at a Time
#
#   CSV.parse("CSV,data,String") do |row|
#     # use row here...
#   end
#
# ==== All at Once
#
#   arr_of_arrs = CSV.parse("CSV,data,String")
#
# == Writing
#
# === To a File
#
#   CSV.open("path/to/file.csv", "wb") do |csv|
#     csv << ["row", "of", "CSV", "data"]
#     csv << ["another", "row"]
#     # ...
#   end
#
# === To a String
#
#   csv_string = CSV.generate do |csv|
#     csv << ["row", "of", "CSV", "data"]
#     csv << ["another", "row"]
#     # ...
#   end
#
# == Convert a Single Line
#
#   csv_string = ["CSV", "data"].to_csv   # to CSV
#   csv_array  = "CSV,String".parse_csv   # from CSV
#
# == Shortcut Interface
#
#   CSV             { |csv_out| csv_out << %w{my data here} }  # to $stdout
#   CSV(csv = "")   { |csv_str| csv_str << %w{my data here} }  # to a String
#   CSV($stderr)    { |csv_err| csv_err << %w{my data here} }  # to $stderr
#   CSV($stdin)     { |csv_in|  csv_in.each { |row| p row } }  # from $stdin
#
# == Advanced Usage
#
# === Wrap an IO Object
#
#   csv = CSV.new(io, options)
#   # ... read (with gets() or each()) from and write (with <<) to csv here ...
#
# == CSV and Character Encodings (M17n or Multilingualization)
#
# This new CSV parser is m17n savvy.  The parser works in the Encoding of the IO
# or String object being read from or written to.  Your data is never transcoded
# (unless you ask Ruby to transcode it for you) and will literally be parsed in
# the Encoding it is in.  Thus CSV will return Arrays or Rows of Strings in the
# Encoding of your data.  This is accomplished by transcoding the parser itself
# into your Encoding.
#
# Some transcoding must take place, of course, to accomplish this multiencoding
# support.  For example, <tt>:col_sep</tt>, <tt>:row_sep</tt>, and
# <tt>:quote_char</tt> must be transcoded to match your data.  Hopefully this
# makes the entire process feel transparent, since CSV's defaults should just
# magically work for you data.  However, you can set these values manually in
# the target Encoding to avoid the translation.
#
# It's also important to note that while all of CSV's core parser is now
# Encoding agnostic, some features are not.  For example, the built-in
# converters will try to transcode data to UTF-8 before making conversions.
# Again, you can provide custom converters that are aware of your Encodings to
# avoid this translation.  It's just too hard for me to support native
# conversions in all of Ruby's Encodings.
#
# Anyway, the practical side of this is simple:  make sure IO and String objects
# passed into CSV have the proper Encoding set and everything should just work.
# CSV methods that allow you to open IO objects (CSV::foreach(), CSV::open(),
# CSV::read(), and CSV::readlines()) do allow you to specify the Encoding.
#
# One minor exception comes when generating CSV into a String with an Encoding
# that is not ASCII compatible.  There's no existing data for CSV to use to
# prepare itself and thus you will probably need to manually specify the desired
# Encoding for most of those cases.  It will try to guess using the fields in a
# row of output though, when using CSV::generate_line() or Array#to_csv().
#
# I try to point out any other Encoding issues in the documentation of methods
# as they come up.
#
# This has been tested to the best of my ability with all non-"dummy" Encodings
# Ruby ships with.  However, it is brave new code and may have some bugs.
# Please feel free to {report}[mailto:james@grayproductions.net] any issues you
# find with it.
#
class CSV
  # The version of the installed library.
  VERSION = "2.4.8".freeze

  #
  # A CSV::Row is part Array and part Hash.  It retains an order for the fields
  # and allows duplicates just as an Array would, but also allows you to access
  # fields by name just as you could if they were in a Hash.
  #
  # All rows returned by CSV will be constructed from this class, if header row
  # processing is activated.
  #
  class Row
    #
    # Construct a new CSV::Row from +headers+ and +fields+, which are expected
    # to be Arrays.  If one Array is shorter than the other, it will be padded
    # with +nil+ objects.
    #
    # The optional +header_row+ parameter can be set to +true+ to indicate, via
    # CSV::Row.header_row?() and CSV::Row.field_row?(), that this is a header
    # row.  Otherwise, the row is assumes to be a field row.
    #
    # A CSV::Row object supports the following Array methods through delegation:
    #
    # * empty?()
    # * length()
    # * size()
    #
    def initialize(headers, fields, header_row = false)
      @header_row = header_row

      # handle extra headers or fields
      @row = if headers.size > fields.size
        headers.zip(fields)
      else
        fields.zip(headers).map { |pair| pair.reverse }
      end
    end

    # Internal data format used to compare equality.
    attr_reader :row
    protected   :row

    ### Array Delegation ###

    extend Forwardable
    def_delegators :@row, :empty?, :length, :size

    # Returns +true+ if this is a header row.
    def header_row?
      @header_row
    end

    # Returns +true+ if this is a field row.
    def field_row?
      not header_row?
    end

    # Returns the headers of this row.
    def headers
      @row.map { |pair| pair.first }
    end

    #
    # :call-seq:
    #   field( header )
    #   field( header, offset )
    #   field( index )
    #
    # This method will fetch the field value by +header+ or +index+.  If a field
    # is not found, +nil+ is returned.
    #
    # When provided, +offset+ ensures that a header match occurrs on or later
    # than the +offset+ index.  You can use this to find duplicate headers,
    # without resorting to hard-coding exact indices.
    #
    def field(header_or_index, minimum_index = 0)
      # locate the pair
      finder = header_or_index.is_a?(Integer) ? :[] : :assoc
      pair   = @row[minimum_index..-1].send(finder, header_or_index)

      # return the field if we have a pair
      pair.nil? ? nil : pair.last
    end
    alias_method :[], :field

    #
    # :call-seq:
    #   []=( header, value )
    #   []=( header, offset, value )
    #   []=( index, value )
    #
    # Looks up the field by the semantics described in CSV::Row.field() and
    # assigns the +value+.
    #
    # Assigning past the end of the row with an index will set all pairs between
    # to <tt>[nil, nil]</tt>.  Assigning to an unused header appends the new
    # pair.
    #
    def []=(*args)
      value = args.pop

      if args.first.is_a? Integer
        if @row[args.first].nil?  # extending past the end with index
          @row[args.first] = [nil, value]
          @row.map! { |pair| pair.nil? ? [nil, nil] : pair }
        else                      # normal index assignment
          @row[args.first][1] = value
        end
      else
        index = index(*args)
        if index.nil?             # appending a field
          self << [args.first, value]
        else                      # normal header assignment
          @row[index][1] = value
        end
      end
    end

    #
    # :call-seq:
    #   <<( field )
    #   <<( header_and_field_array )
    #   <<( header_and_field_hash )
    #
    # If a two-element Array is provided, it is assumed to be a header and field
    # and the pair is appended.  A Hash works the same way with the key being
    # the header and the value being the field.  Anything else is assumed to be
    # a lone field which is appended with a +nil+ header.
    #
    # This method returns the row for chaining.
    #
    def <<(arg)
      if arg.is_a?(Array) and arg.size == 2  # appending a header and name
        @row << arg
      elsif arg.is_a?(Hash)                  # append header and name pairs
        arg.each { |pair| @row << pair }
      else                                   # append field value
        @row << [nil, arg]
      end

      self  # for chaining
    end

    #
    # A shortcut for appending multiple fields.  Equivalent to:
    #
    #   args.each { |arg| csv_row << arg }
    #
    # This method returns the row for chaining.
    #
    def push(*args)
      args.each { |arg| self << arg }

      self  # for chaining
    end

    #
    # :call-seq:
    #   delete( header )
    #   delete( header, offset )
    #   delete( index )
    #
    # Used to remove a pair from the row by +header+ or +index+.  The pair is
    # located as described in CSV::Row.field().  The deleted pair is returned,
    # or +nil+ if a pair could not be found.
    #
    def delete(header_or_index, minimum_index = 0)
      if header_or_index.is_a? Integer                 # by index
        @row.delete_at(header_or_index)
      elsif i = index(header_or_index, minimum_index)  # by header
        @row.delete_at(i)
      else
        [ ]
      end
    end

    #
    # The provided +block+ is passed a header and field for each pair in the row
    # and expected to return +true+ or +false+, depending on whether the pair
    # should be deleted.
    #
    # This method returns the row for chaining.
    #
    def delete_if(&block)
      @row.delete_if(&block)

      self  # for chaining
    end

    #
    # This method accepts any number of arguments which can be headers, indices,
    # Ranges of either, or two-element Arrays containing a header and offset.
    # Each argument will be replaced with a field lookup as described in
    # CSV::Row.field().
    #
    # If called with no arguments, all fields are returned.
    #
    def fields(*headers_and_or_indices)
      if headers_and_or_indices.empty?  # return all fields--no arguments
        @row.map { |pair| pair.last }
      else                              # or work like values_at()
        headers_and_or_indices.inject(Array.new) do |all, h_or_i|
          all + if h_or_i.is_a? Range
            index_begin = h_or_i.begin.is_a?(Integer) ? h_or_i.begin :
                                                        index(h_or_i.begin)
            index_end   = h_or_i.end.is_a?(Integer)   ? h_or_i.end :
                                                        index(h_or_i.end)
            new_range   = h_or_i.exclude_end? ? (index_begin...index_end) :
                                                (index_begin..index_end)
            fields.values_at(new_range)
          else
            [field(*Array(h_or_i))]
          end
        end
      end
    end
    alias_method :values_at, :fields

    #
    # :call-seq:
    #   index( header )
    #   index( header, offset )
    #
    # This method will return the index of a field with the provided +header+.
    # The +offset+ can be used to locate duplicate header names, as described in
    # CSV::Row.field().
    #
    def index(header, minimum_index = 0)
      # find the pair
      index = headers[minimum_index..-1].index(header)
      # return the index at the right offset, if we found one
      index.nil? ? nil : index + minimum_index
    end

    # Returns +true+ if +name+ is a header for this row, and +false+ otherwise.
    def header?(name)
      headers.include? name
    end
    alias_method :include?, :header?

    #
    # Returns +true+ if +data+ matches a field in this row, and +false+
    # otherwise.
    #
    def field?(data)
      fields.include? data
    end

    include Enumerable

    #
    # Yields each pair of the row as header and field tuples (much like
    # iterating over a Hash).
    #
    # Support for Enumerable.
    #
    # This method returns the row for chaining.
    #
    def each(&block)
      @row.each(&block)

      self  # for chaining
    end

    #
    # Returns +true+ if this row contains the same headers and fields in the
    # same order as +other+.
    #
    def ==(other)
      @row == other.row
    end

    #
    # Collapses the row into a simple Hash.  Be warning that this discards field
    # order and clobbers duplicate fields.
    #
    def to_hash
      # flatten just one level of the internal Array
      Hash[*@row.inject(Array.new) { |ary, pair| ary.push(*pair) }]
    end

    #
    # Returns the row as a CSV String.  Headers are not used.  Equivalent to:
    #
    #   csv_row.fields.to_csv( options )
    #
    def to_csv(options = Hash.new)
      fields.to_csv(options)
    end
    alias_method :to_s, :to_csv

    # A summary of fields, by header, in an ASCII compatible String.
    def inspect
      str = ["#<", self.class.to_s]
      each do |header, field|
        str << " " << (header.is_a?(Symbol) ? header.to_s : header.inspect) <<
               ":" << field.inspect
      end
      str << ">"
      begin
        str.join('')
      rescue  # any encoding error
        str.map do |s|
          e = Encoding::Converter.asciicompat_encoding(s.encoding)
          e ? s.encode(e) : s.force_encoding("ASCII-8BIT")
        end.join('')
      end
    end
  end

  #
  # A CSV::Table is a two-dimensional data structure for representing CSV
  # documents.  Tables allow you to work with the data by row or column,
  # manipulate the data, and even convert the results back to CSV, if needed.
  #
  # All tables returned by CSV will be constructed from this class, if header
  # row processing is activated.
  #
  class Table
    #
    # Construct a new CSV::Table from +array_of_rows+, which are expected
    # to be CSV::Row objects.  All rows are assumed to have the same headers.
    #
    # A CSV::Table object supports the following Array methods through
    # delegation:
    #
    # * empty?()
    # * length()
    # * size()
    #
    def initialize(array_of_rows)
      @table = array_of_rows
      @mode  = :col_or_row
    end

    # The current access mode for indexing and iteration.
    attr_reader :mode

    # Internal data format used to compare equality.
    attr_reader :table
    protected   :table

    ### Array Delegation ###

    extend Forwardable
    def_delegators :@table, :empty?, :length, :size

    #
    # Returns a duplicate table object, in column mode.  This is handy for
    # chaining in a single call without changing the table mode, but be aware
    # that this method can consume a fair amount of memory for bigger data sets.
    #
    # This method returns the duplicate table for chaining.  Don't chain
    # destructive methods (like []=()) this way though, since you are working
    # with a duplicate.
    #
    def by_col
      self.class.new(@table.dup).by_col!
    end

    #
    # Switches the mode of this table to column mode.  All calls to indexing and
    # iteration methods will work with columns until the mode is changed again.
    #
    # This method returns the table and is safe to chain.
    #
    def by_col!
      @mode = :col

      self
    end

    #
    # Returns a duplicate table object, in mixed mode.  This is handy for
    # chaining in a single call without changing the table mode, but be aware
    # that this method can consume a fair amount of memory for bigger data sets.
    #
    # This method returns the duplicate table for chaining.  Don't chain
    # destructive methods (like []=()) this way though, since you are working
    # with a duplicate.
    #
    def by_col_or_row
      self.class.new(@table.dup).by_col_or_row!
    end

    #
    # Switches the mode of this table to mixed mode.  All calls to indexing and
    # iteration methods will use the default intelligent indexing system until
    # the mode is changed again.  In mixed mode an index is assumed to be a row
    # reference while anything else is assumed to be column access by headers.
    #
    # This method returns the table and is safe to chain.
    #
    def by_col_or_row!
      @mode = :col_or_row

      self
    end

    #
    # Returns a duplicate table object, in row mode.  This is handy for chaining
    # in a single call without changing the table mode, but be aware that this
    # method can consume a fair amount of memory for bigger data sets.
    #
    # This method returns the duplicate table for chaining.  Don't chain
    # destructive methods (like []=()) this way though, since you are working
    # with a duplicate.
    #
    def by_row
      self.class.new(@table.dup).by_row!
    end

    #
    # Switches the mode of this table to row mode.  All calls to indexing and
    # iteration methods will work with rows until the mode is changed again.
    #
    # This method returns the table and is safe to chain.
    #
    def by_row!
      @mode = :row

      self
    end

    #
    # Returns the headers for the first row of this table (assumed to match all
    # other rows).  An empty Array is returned for empty tables.
    #
    def headers
      if @table.empty?
        Array.new
      else
        @table.first.headers
      end
    end

    #
    # In the default mixed mode, this method returns rows for index access and
    # columns for header access.  You can force the index association by first
    # calling by_col!() or by_row!().
    #
    # Columns are returned as an Array of values.  Altering that Array has no
    # effect on the table.
    #
    def [](index_or_header)
      if @mode == :row or  # by index
         (@mode == :col_or_row and index_or_header.is_a? Integer)
        @table[index_or_header]
      else                 # by header
        @table.map { |row| row[index_or_header] }
      end
    end

    #
    # In the default mixed mode, this method assigns rows for index access and
    # columns for header access.  You can force the index association by first
    # calling by_col!() or by_row!().
    #
    # Rows may be set to an Array of values (which will inherit the table's
    # headers()) or a CSV::Row.
    #
    # Columns may be set to a single value, which is copied to each row of the
    # column, or an Array of values.  Arrays of values are assigned to rows top
    # to bottom in row major order.  Excess values are ignored and if the Array
    # does not have a value for each row the extra rows will receive a +nil+.
    #
    # Assigning to an existing column or row clobbers the data.  Assigning to
    # new columns creates them at the right end of the table.
    #
    def []=(index_or_header, value)
      if @mode == :row or  # by index
         (@mode == :col_or_row and index_or_header.is_a? Integer)
        if value.is_a? Array
          @table[index_or_header] = Row.new(headers, value)
        else
          @table[index_or_header] = value
        end
      else                 # set column
        if value.is_a? Array  # multiple values
          @table.each_with_index do |row, i|
            if row.header_row?
              row[index_or_header] = index_or_header
            else
              row[index_or_header] = value[i]
            end
          end
        else                  # repeated value
          @table.each do |row|
            if row.header_row?
              row[index_or_header] = index_or_header
            else
              row[index_or_header] = value
            end
          end
        end
      end
    end

    #
    # The mixed mode default is to treat a list of indices as row access,
    # returning the rows indicated.  Anything else is considered columnar
    # access.  For columnar access, the return set has an Array for each row
    # with the values indicated by the headers in each Array.  You can force
    # column or row mode using by_col!() or by_row!().
    #
    # You cannot mix column and row access.
    #
    def values_at(*indices_or_headers)
      if @mode == :row or  # by indices
         ( @mode == :col_or_row and indices_or_headers.all? do |index|
                                      index.is_a?(Integer)         or
                                      ( index.is_a?(Range)         and
                                        index.first.is_a?(Integer) and
                                        index.last.is_a?(Integer) )
                                    end )
        @table.values_at(*indices_or_headers)
      else                 # by headers
        @table.map { |row| row.values_at(*indices_or_headers) }
      end
    end

    #
    # Adds a new row to the bottom end of this table.  You can provide an Array,
    # which will be converted to a CSV::Row (inheriting the table's headers()),
    # or a CSV::Row.
    #
    # This method returns the table for chaining.
    #
    def <<(row_or_array)
      if row_or_array.is_a? Array  # append Array
        @table << Row.new(headers, row_or_array)
      else                         # append Row
        @table << row_or_array
      end

      self  # for chaining
    end

    #
    # A shortcut for appending multiple rows.  Equivalent to:
    #
    #   rows.each { |row| self << row }
    #
    # This method returns the table for chaining.
    #
    def push(*rows)
      rows.each { |row| self << row }

      self  # for chaining
    end

    #
    # Removes and returns the indicated column or row.  In the default mixed
    # mode indices refer to rows and everything else is assumed to be a column
    # header.  Use by_col!() or by_row!() to force the lookup.
    #
    def delete(index_or_header)
      if @mode == :row or  # by index
         (@mode == :col_or_row and index_or_header.is_a? Integer)
        @table.delete_at(index_or_header)
      else                 # by header
        @table.map { |row| row.delete(index_or_header).last }
      end
    end

    #
    # Removes any column or row for which the block returns +true+.  In the
    # default mixed mode or row mode, iteration is the standard row major
    # walking of rows.  In column mode, interation will +yield+ two element
    # tuples containing the column name and an Array of values for that column.
    #
    # This method returns the table for chaining.
    #
    def delete_if(&block)
      if @mode == :row or @mode == :col_or_row  # by index
        @table.delete_if(&block)
      else                                      # by header
        to_delete = Array.new
        headers.each_with_index do |header, i|
          to_delete << header if block[[header, self[header]]]
        end
        to_delete.map { |header| delete(header) }
      end

      self  # for chaining
    end

    include Enumerable

    #
    # In the default mixed mode or row mode, iteration is the standard row major
    # walking of rows.  In column mode, interation will +yield+ two element
    # tuples containing the column name and an Array of values for that column.
    #
    # This method returns the table for chaining.
    #
    def each(&block)
      if @mode == :col
        headers.each { |header| block[[header, self[header]]] }
      else
        @table.each(&block)
      end

      self  # for chaining
    end

    # Returns +true+ if all rows of this table ==() +other+'s rows.
    def ==(other)
      @table == other.table
    end

    #
    # Returns the table as an Array of Arrays.  Headers will be the first row,
    # then all of the field rows will follow.
    #
    def to_a
      @table.inject([headers]) do |array, row|
        if row.header_row?
          array
        else
          array + [row.fields]
        end
      end
    end

    #
    # Returns the table as a complete CSV String.  Headers will be listed first,
    # then all of the field rows.
    #
    # This method assumes you want the Table.headers(), unless you explicitly
    # pass <tt>:write_headers => false</tt>.
    #
    def to_csv(options = Hash.new)
      wh = options.fetch(:write_headers, true)
      @table.inject(wh ? [headers.to_csv(options)] : [ ]) do |rows, row|
        if row.header_row?
          rows
        else
          rows + [row.fields.to_csv(options)]
        end
      end.join('')
    end
    alias_method :to_s, :to_csv

    # Shows the mode and size of this table in a US-ASCII String.
    def inspect
      "#<#{self.class} mode:#{@mode} row_count:#{to_a.size}>".encode("US-ASCII")
    end
  end

  # The error thrown when the parser encounters illegal CSV formatting.
  class MalformedCSVError < RuntimeError; end

  #
  # A FieldInfo Struct contains details about a field's position in the data
  # source it was read from.  CSV will pass this Struct to some blocks that make
  # decisions based on field structure.  See CSV.convert_fields() for an
  # example.
  #
  # <b><tt>index</tt></b>::  The zero-based index of the field in its row.
  # <b><tt>line</tt></b>::   The line of the data source this row is from.
  # <b><tt>header</tt></b>:: The header for the column, when available.
  #
  FieldInfo = Struct.new(:index, :line, :header)

  # A Regexp used to find and convert some common Date formats.
  DateMatcher     = / \A(?: (\w+,?\s+)?\w+\s+\d{1,2},?\s+\d{2,4} |
                            \d{4}-\d{2}-\d{2} )\z /x
  # A Regexp used to find and convert some common DateTime formats.
  DateTimeMatcher =
    / \A(?: (\w+,?\s+)?\w+\s+\d{1,2}\s+\d{1,2}:\d{1,2}:\d{1,2},?\s+\d{2,4} |
            \d{4}-\d{2}-\d{2}\s\d{2}:\d{2}:\d{2} )\z /x

  # The encoding used by all converters.
  ConverterEncoding = Encoding.find("UTF-8")

  #
  # This Hash holds the built-in converters of CSV that can be accessed by name.
  # You can select Converters with CSV.convert() or through the +options+ Hash
  # passed to CSV::new().
  #
  # <b><tt>:integer</tt></b>::    Converts any field Integer() accepts.
  # <b><tt>:float</tt></b>::      Converts any field Float() accepts.
  # <b><tt>:numeric</tt></b>::    A combination of <tt>:integer</tt>
  #                               and <tt>:float</tt>.
  # <b><tt>:date</tt></b>::       Converts any field Date::parse() accepts.
  # <b><tt>:date_time</tt></b>::  Converts any field DateTime::parse() accepts.
  # <b><tt>:all</tt></b>::        All built-in converters.  A combination of
  #                               <tt>:date_time</tt> and <tt>:numeric</tt>.
  #
  # All built-in converters transcode field data to UTF-8 before attempting a
  # conversion.  If your data cannot be transcoded to UTF-8 the conversion will
  # fail and the field will remain unchanged.
  #
  # This Hash is intentionally left unfrozen and users should feel free to add
  # values to it that can be accessed by all CSV objects.
  #
  # To add a combo field, the value should be an Array of names.  Combo fields
  # can be nested with other combo fields.
  #
  Converters  = { integer:   lambda { |f|
                    Integer(f.encode(ConverterEncoding)) rescue f
                  },
                  float:     lambda { |f|
                    Float(f.encode(ConverterEncoding)) rescue f
                  },
                  numeric:   [:integer, :float],
                  date:      lambda { |f|
                    begin
                      e = f.encode(ConverterEncoding)
                      e =~ DateMatcher ? Date.parse(e) : f
                    rescue  # encoding conversion or date parse errors
                      f
                    end
                  },
                  date_time: lambda { |f|
                    begin
                      e = f.encode(ConverterEncoding)
                      e =~ DateTimeMatcher ? DateTime.parse(e) : f
                    rescue  # encoding conversion or date parse errors
                      f
                    end
                  },
                  all:       [:date_time, :numeric] }

  #
  # This Hash holds the built-in header converters of CSV that can be accessed
  # by name.  You can select HeaderConverters with CSV.header_convert() or
  # through the +options+ Hash passed to CSV::new().
  #
  # <b><tt>:downcase</tt></b>::  Calls downcase() on the header String.
  # <b><tt>:symbol</tt></b>::    The header String is downcased, spaces are
  #                              replaced with underscores, non-word characters
  #                              are dropped, and finally to_sym() is called.
  #
  # All built-in header converters transcode header data to UTF-8 before
  # attempting a conversion.  If your data cannot be transcoded to UTF-8 the
  # conversion will fail and the header will remain unchanged.
  #
  # This Hash is intetionally left unfrozen and users should feel free to add
  # values to it that can be accessed by all CSV objects.
  #
  # To add a combo field, the value should be an Array of names.  Combo fields
  # can be nested with other combo fields.
  #
  HeaderConverters = {
    downcase: lambda { |h| h.encode(ConverterEncoding).downcase },
    symbol:   lambda { |h|
      h.encode(ConverterEncoding).downcase.gsub(/\s+/, "_").
                                           gsub(/\W+/, "").to_sym
    }
  }

  #
  # The options used when no overrides are given by calling code.  They are:
  #
  # <b><tt>:col_sep</tt></b>::            <tt>","</tt>
  # <b><tt>:row_sep</tt></b>::            <tt>:auto</tt>
  # <b><tt>:quote_char</tt></b>::         <tt>'"'</tt>
  # <b><tt>:field_size_limit</tt></b>::   +nil+
  # <b><tt>:converters</tt></b>::         +nil+
  # <b><tt>:unconverted_fields</tt></b>:: +nil+
  # <b><tt>:headers</tt></b>::            +false+
  # <b><tt>:return_headers</tt></b>::     +false+
  # <b><tt>:header_converters</tt></b>::  +nil+
  # <b><tt>:skip_blanks</tt></b>::        +false+
  # <b><tt>:force_quotes</tt></b>::       +false+
  #
  DEFAULT_OPTIONS = { col_sep:            ",",
                      row_sep:            :auto,
                      quote_char:         '"',
                      field_size_limit:   nil,
                      converters:         nil,
                      unconverted_fields: nil,
                      headers:            false,
                      return_headers:     false,
                      header_converters:  nil,
                      skip_blanks:        false,
                      force_quotes:       false }.freeze

  #
  # This method will return a CSV instance, just like CSV::new(), but the
  # instance will be cached and returned for all future calls to this method for
  # the same +data+ object (tested by Object#object_id()) with the same
  # +options+.
  #
  # If a block is given, the instance is passed to the block and the return
  # value becomes the return value of the block.
  #
  def self.instance(data = $stdout, options = Hash.new)
    # create a _signature_ for this method call, data object and options
    sig = [data.object_id] +
          options.values_at(*DEFAULT_OPTIONS.keys.sort_by { |sym| sym.to_s })

    # fetch or create the instance for this signature
    @@instances ||= Hash.new
    instance    =   (@@instances[sig] ||= new(data, options))

    if block_given?
      yield instance  # run block, if given, returning result
    else
      instance        # or return the instance
    end
  end

  #
  # This method allows you to serialize an Array of Ruby objects to a String or
  # File of CSV data.  This is not as powerful as Marshal or YAML, but perhaps
  # useful for spreadsheet and database interaction.
  #
  # Out of the box, this method is intended to work with simple data objects or
  # Structs.  It will serialize a list of instance variables and/or
  # Struct.members().
  #
  # If you need need more complicated serialization, you can control the process
  # by adding methods to the class to be serialized.
  #
  # A class method csv_meta() is responsible for returning the first row of the
  # document (as an Array).  This row is considered to be a Hash of the form
  # key_1,value_1,key_2,value_2,...  CSV::load() expects to find a class key
  # with a value of the stringified class name and CSV::dump() will create this,
  # if you do not define this method.  This method is only called on the first
  # object of the Array.
  #
  # The next method you can provide is an instance method called csv_headers().
  # This method is expected to return the second line of the document (again as
  # an Array), which is to be used to give each column a header.  By default,
  # CSV::load() will set an instance variable if the field header starts with an
  # @ character or call send() passing the header as the method name and
  # the field value as an argument.  This method is only called on the first
  # object of the Array.
  #
  # Finally, you can provide an instance method called csv_dump(), which will
  # be passed the headers.  This should return an Array of fields that can be
  # serialized for this object.  This method is called once for every object in
  # the Array.
  #
  # The +io+ parameter can be used to serialize to a File, and +options+ can be
  # anything CSV::new() accepts.
  #
  def self.dump(ary_of_objs, io = "", options = Hash.new)
    obj_template = ary_of_objs.first

    csv = new(io, options)

    # write meta information
    begin
      csv << obj_template.class.csv_meta
    rescue NoMethodError
      csv << [:class, obj_template.class]
    end

    # write headers
    begin
      headers = obj_template.csv_headers
    rescue NoMethodError
      headers = obj_template.instance_variables.sort
      if obj_template.class.ancestors.find { |cls| cls.to_s =~ /\AStruct\b/ }
        headers += obj_template.members.map { |mem| "#{mem}=" }.sort
      end
    end
    csv << headers

    # serialize each object
    ary_of_objs.each do |obj|
      begin
        csv << obj.csv_dump(headers)
      rescue NoMethodError
        csv << headers.map do |var|
          if var[0] == ?@
            obj.instance_variable_get(var)
          else
            obj[var[0..-2]]
          end
        end
      end
    end

    if io.is_a? String
      csv.string
    else
      csv.close
    end
  end

  #
  # This method is the reading counterpart to CSV::dump().  See that method for
  # a detailed description of the process.
  #
  # You can customize loading by adding a class method called csv_load() which
  # will be passed a Hash of meta information, an Array of headers, and an Array
  # of fields for the object the method is expected to return.
  #
  # Remember that all fields will be Strings after this load.  If you need
  # something else, use +options+ to setup converters or provide a custom
  # csv_load() implementation.
  #
  def self.load(io_or_str, options = Hash.new)
    csv = new(io_or_str, options)

    # load meta information
    meta = Hash[*csv.shift]
    cls  = meta["class".encode(csv.encoding)].split("::".encode(csv.encoding)).
                                              inject(Object) do |c, const|
      c.const_get(const)
    end

    # load headers
    headers = csv.shift

    # unserialize each object stored in the file
    results = csv.inject(Array.new) do |all, row|
      begin
        obj = cls.csv_load(meta, headers, row)
      rescue NoMethodError
        obj = cls.allocate
        headers.zip(row) do |name, value|
          if name[0] == ?@
            obj.instance_variable_set(name, value)
          else
            obj.send(name, value)
          end
        end
      end
      all << obj
    end

    csv.close unless io_or_str.is_a? String

    results
  end

  #
  # :call-seq:
  #   filter( options = Hash.new ) { |row| ... }
  #   filter( input, options = Hash.new ) { |row| ... }
  #   filter( input, output, options = Hash.new ) { |row| ... }
  #
  # This method is a convenience for building Unix-like filters for CSV data.
  # Each row is yielded to the provided block which can alter it as needed.
  # After the block returns, the row is appended to +output+ altered or not.
  #
  # The +input+ and +output+ arguments can be anything CSV::new() accepts
  # (generally String or IO objects).  If not given, they default to
  # <tt>ARGF</tt> and <tt>$stdout</tt>.
  #
  # The +options+ parameter is also filtered down to CSV::new() after some
  # clever key parsing.  Any key beginning with <tt>:in_</tt> or
  # <tt>:input_</tt> will have that leading identifier stripped and will only
  # be used in the +options+ Hash for the +input+ object.  Keys starting with
  # <tt>:out_</tt> or <tt>:output_</tt> affect only +output+.  All other keys
  # are assigned to both objects.
  #
  # The <tt>:output_row_sep</tt> +option+ defaults to
  # <tt>$INPUT_RECORD_SEPARATOR</tt> (<tt>$/</tt>).
  #
  def self.filter(*args)
    # parse options for input, output, or both
    in_options, out_options = Hash.new, {row_sep: $INPUT_RECORD_SEPARATOR}
    if args.last.is_a? Hash
      args.pop.each do |key, value|
        case key.to_s
        when /\Ain(?:put)?_(.+)\Z/
          in_options[$1.to_sym] = value
        when /\Aout(?:put)?_(.+)\Z/
          out_options[$1.to_sym] = value
        else
          in_options[key]  = value
          out_options[key] = value
        end
      end
    end
    # build input and output wrappers
    input  = new(args.shift || ARGF,    in_options)
    output = new(args.shift || $stdout, out_options)

    # read, yield, write
    input.each do |row|
      yield row
      output << row
    end
  end

  #
  # This method is intended as the primary interface for reading CSV files.  You
  # pass a +path+ and any +options+ you wish to set for the read.  Each row of
  # file will be passed to the provided +block+ in turn.
  #
  # The +options+ parameter can be anything CSV::new() understands.  This method
  # also understands an additional <tt>:encoding</tt> parameter that you can use
  # to specify the Encoding of the data in the file to be read. You must provide
  # this unless your data is in Encoding::default_external().  CSV will use this
  # to determine how to parse the data.  You may provide a second Encoding to
  # have the data transcoded as it is read.  For example,
  # <tt>encoding: "UTF-32BE:UTF-8"</tt> would read UTF-32BE data from the file
  # but transcode it to UTF-8 before CSV parses it.
  #
  def self.foreach(path, options = Hash.new, &block)
    open(path, options) do |csv|
      csv.each(&block)
    end
  end

  #
  # :call-seq:
  #   generate( str, options = Hash.new ) { |csv| ... }
  #   generate( options = Hash.new ) { |csv| ... }
  #
  # This method wraps a String you provide, or an empty default String, in a
  # CSV object which is passed to the provided block.  You can use the block to
  # append CSV rows to the String and when the block exits, the final String
  # will be returned.
  #
  # Note that a passed String *is* modfied by this method.  Call dup() before
  # passing if you need a new String.
  #
  # The +options+ parameter can be anything CSV::new() understands.  This method
  # understands an additional <tt>:encoding</tt> parameter when not passed a
  # String to set the base Encoding for the output.  CSV needs this hint if you
  # plan to output non-ASCII compatible data.
  #
  def self.generate(*args)
    # add a default empty String, if none was given
    if args.first.is_a? String
      io = StringIO.new(args.shift)
      io.seek(0, IO::SEEK_END)
      args.unshift(io)
    else
      encoding = (args[-1] = args[-1].dup).delete(:encoding) if args.last.is_a?(Hash)
      str      = ""
      str.encode!(encoding) if encoding
      args.unshift(str)
    end
    csv = new(*args)  # wrap
    yield csv         # yield for appending
    csv.string        # return final String
  end

  #
  # This method is a shortcut for converting a single row (Array) into a CSV
  # String.
  #
  # The +options+ parameter can be anything CSV::new() understands.  This method
  # understands an additional <tt>:encoding</tt> parameter to set the base
  # Encoding for the output.  This method will try to guess your Encoding from
  # the first non-+nil+ field in +row+, if possible, but you may need to use
  # this parameter as a backup plan.
  #
  # The <tt>:row_sep</tt> +option+ defaults to <tt>$INPUT_RECORD_SEPARATOR</tt>
  # (<tt>$/</tt>) when calling this method.
  #
  def self.generate_line(row, options = Hash.new)
    options  = {row_sep: $INPUT_RECORD_SEPARATOR}.merge(options)
    encoding = options.delete(:encoding)
    str      = ""
    if encoding
      str.force_encoding(encoding)
    elsif field = row.find { |f| not f.nil? }
      str.force_encoding(String(field).encoding)
    end
    (new(str, options) << row).string
  end

  #
  # :call-seq:
  #   open( filename, mode = "rb", options = Hash.new ) { |faster_csv| ... }
  #   open( filename, options = Hash.new ) { |faster_csv| ... }
  #   open( filename, mode = "rb", options = Hash.new )
  #   open( filename, options = Hash.new )
  #
  # This method opens an IO object, and wraps that with CSV.  This is intended
  # as the primary interface for writing a CSV file.
  #
  # You must pass a +filename+ and may optionally add a +mode+ for Ruby's
  # open().  You may also pass an optional Hash containing any +options+
  # CSV::new() understands as the final argument.
  #
  # This method works like Ruby's open() call, in that it will pass a CSV object
  # to a provided block and close it when the block terminates, or it will
  # return the CSV object when no block is provided.  (*Note*: This is different
  # from the Ruby 1.8 CSV library which passed rows to the block.  Use
  # CSV::foreach() for that behavior.)
  #
  # You must provide a +mode+ with an embedded Encoding designator unless your
  # data is in Encoding::default_external().  CSV will check the Encoding of the
  # underlying IO object (set by the +mode+ you pass) to determine how to parse
  # the data.   You may provide a second Encoding to have the data transcoded as
  # it is read just as you can with a normal call to IO::open().  For example,
  # <tt>"rb:UTF-32BE:UTF-8"</tt> would read UTF-32BE data from the file but
  # transcode it to UTF-8 before CSV parses it.
  #
  # An opened CSV object will delegate to many IO methods for convenience.  You
  # may call:
  #
  # * binmode()
  # * binmode?()
  # * close()
  # * close_read()
  # * close_write()
  # * closed?()
  # * eof()
  # * eof?()
  # * external_encoding()
  # * fcntl()
  # * fileno()
  # * flock()
  # * flush()
  # * fsync()
  # * internal_encoding()
  # * ioctl()
  # * isatty()
  # * path()
  # * pid()
  # * pos()
  # * pos=()
  # * reopen()
  # * seek()
  # * stat()
  # * sync()
  # * sync=()
  # * tell()
  # * to_i()
  # * to_io()
  # * truncate()
  # * tty?()
  #
  def self.open(*args)
    # find the +options+ Hash
    options = if args.last.is_a? Hash then args.pop else Hash.new end
    # wrap a File opened with the remaining +args+ with no newline
    # decorator
    file_opts = {universal_newline: false}.merge(options)
    begin
      f = File.open(*args, file_opts)
    rescue ArgumentError => e
      raise unless /needs binmode/ =~ e.message and args.size == 1
      args << "rb"
      file_opts = {encoding: Encoding.default_external}.merge(file_opts)
      retry
    end
    csv = new(f, options)

    # handle blocks like Ruby's open(), not like the CSV library
    if block_given?
      begin
        yield csv
      ensure
        csv.close
      end
    else
      csv
    end
  end

  #
  # :call-seq:
  #   parse( str, options = Hash.new ) { |row| ... }
  #   parse( str, options = Hash.new )
  #
  # This method can be used to easily parse CSV out of a String.  You may either
  # provide a +block+ which will be called with each row of the String in turn,
  # or just use the returned Array of Arrays (when no +block+ is given).
  #
  # You pass your +str+ to read from, and an optional +options+ Hash containing
  # anything CSV::new() understands.
  #
  def self.parse(*args, &block)
    csv = new(*args)
    if block.nil?  # slurp contents, if no block is given
      begin
        csv.read
      ensure
        csv.close
      end
    else           # or pass each row to a provided block
      csv.each(&block)
    end
  end

  #
  # This method is a shortcut for converting a single line of a CSV String into
  # a into an Array.  Note that if +line+ contains multiple rows, anything
  # beyond the first row is ignored.
  #
  # The +options+ parameter can be anything CSV::new() understands.
  #
  def self.parse_line(line, options = Hash.new)
    new(line, options).shift
  end

  #
  # Use to slurp a CSV file into an Array of Arrays.  Pass the +path+ to the
  # file and any +options+ CSV::new() understands.  This method also understands
  # an additional <tt>:encoding</tt> parameter that you can use to specify the
  # Encoding of the data in the file to be read. You must provide this unless
  # your data is in Encoding::default_external().  CSV will use this to determine
  # how to parse the data.  You may provide a second Encoding to have the data
  # transcoded as it is read.  For example,
  # <tt>encoding: "UTF-32BE:UTF-8"</tt> would read UTF-32BE data from the file
  # but transcode it to UTF-8 before CSV parses it.
  #
  def self.read(path, *options)
    open(path, *options) { |csv| csv.read }
  end

  # Alias for CSV::read().
  def self.readlines(*args)
    read(*args)
  end

  #
  # A shortcut for:
  #
  #   CSV.read( path, { headers:           true,
  #                     converters:        :numeric,
  #                     header_converters: :symbol }.merge(options) )
  #
  def self.table(path, options = Hash.new)
    read( path, { headers:           true,
                  converters:        :numeric,
                  header_converters: :symbol }.merge(options) )
  end

  #
  # This constructor will wrap either a String or IO object passed in +data+ for
  # reading and/or writing.  In addition to the CSV instance methods, several IO
  # methods are delegated.  (See CSV::open() for a complete list.)  If you pass
  # a String for +data+, you can later retrieve it (after writing to it, for
  # example) with CSV.string().
  #
  # Note that a wrapped String will be positioned at at the beginning (for
  # reading).  If you want it at the end (for writing), use CSV::generate().
  # If you want any other positioning, pass a preset StringIO object instead.
  #
  # You may set any reading and/or writing preferences in the +options+ Hash.
  # Available options are:
  #
  # <b><tt>:col_sep</tt></b>::            The String placed between each field.
  #                                       This String will be transcoded into
  #                                       the data's Encoding before parsing.
  # <b><tt>:row_sep</tt></b>::            The String appended to the end of each
  #                                       row.  This can be set to the special
  #                                       <tt>:auto</tt> setting, which requests
  #                                       that CSV automatically discover this
  #                                       from the data.  Auto-discovery reads
  #                                       ahead in the data looking for the next
  #                                       <tt>"\r\n"</tt>, <tt>"\n"</tt>, or
  #                                       <tt>"\r"</tt> sequence.  A sequence
  #                                       will be selected even if it occurs in
  #                                       a quoted field, assuming that you
  #                                       would have the same line endings
  #                                       there.  If none of those sequences is
  #                                       found, +data+ is <tt>ARGF</tt>,
  #                                       <tt>STDIN</tt>, <tt>STDOUT</tt>, or
  #                                       <tt>STDERR</tt>, or the stream is only
  #                                       available for output, the default
  #                                       <tt>$INPUT_RECORD_SEPARATOR</tt>
  #                                       (<tt>$/</tt>) is used.  Obviously,
  #                                       discovery takes a little time.  Set
  #                                       manually if speed is important.  Also
  #                                       note that IO objects should be opened
  #                                       in binary mode on Windows if this
  #                                       feature will be used as the
  #                                       line-ending translation can cause
  #                                       problems with resetting the document
  #                                       position to where it was before the
  #                                       read ahead. This String will be
  #                                       transcoded into the data's Encoding
  #                                       before parsing.
  # <b><tt>:quote_char</tt></b>::         The character used to quote fields.
  #                                       This has to be a single character
  #                                       String.  This is useful for
  #                                       application that incorrectly use
  #                                       <tt>'</tt> as the quote character
  #                                       instead of the correct <tt>"</tt>.
  #                                       CSV will always consider a double
  #                                       sequence this character to be an
  #                                       escaped quote. This String will be
  #                                       transcoded into the data's Encoding
  #                                       before parsing.
  # <b><tt>:field_size_limit</tt></b>::   This is a maximum size CSV will read
  #                                       ahead looking for the closing quote
  #                                       for a field.  (In truth, it reads to
  #                                       the first line ending beyond this
  #                                       size.)  If a quote cannot be found
  #                                       within the limit CSV will raise a
  #                                       MalformedCSVError, assuming the data
  #                                       is faulty.  You can use this limit to
  #                                       prevent what are effectively DoS
  #                                       attacks on the parser.  However, this
  #                                       limit can cause a legitimate parse to
  #                                       fail and thus is set to +nil+, or off,
  #                                       by default.
  # <b><tt>:converters</tt></b>::         An Array of names from the Converters
  #                                       Hash and/or lambdas that handle custom
  #                                       conversion.  A single converter
  #                                       doesn't have to be in an Array.  All
  #                                       built-in converters try to transcode
  #                                       fields to UTF-8 before converting.
  #                                       The conversion will fail if the data
  #                                       cannot be transcoded, leaving the
  #                                       field unchanged.
  # <b><tt>:unconverted_fields</tt></b>:: If set to +true+, an
  #                                       unconverted_fields() method will be
  #                                       added to all returned rows (Array or
  #                                       CSV::Row) that will return the fields
  #                                       as they were before conversion.  Note
  #                                       that <tt>:headers</tt> supplied by
  #                                       Array or String were not fields of the
  #                                       document and thus will have an empty
  #                                       Array attached.
  # <b><tt>:headers</tt></b>::            If set to <tt>:first_row</tt> or
  #                                       +true+, the initial row of the CSV
  #                                       file will be treated as a row of
  #                                       headers.  If set to an Array, the
  #                                       contents will be used as the headers.
  #                                       If set to a String, the String is run
  #                                       through a call of CSV::parse_line()
  #                                       with the same <tt>:col_sep</tt>,
  #                                       <tt>:row_sep</tt>, and
  #                                       <tt>:quote_char</tt> as this instance
  #                                       to produce an Array of headers.  This
  #                                       setting causes CSV#shift() to return
  #                                       rows as CSV::Row objects instead of
  #                                       Arrays and CSV#read() to return
  #                                       CSV::Table objects instead of an Array
  #                                       of Arrays.
  # <b><tt>:return_headers</tt></b>::     When +false+, header rows are silently
  #                                       swallowed.  If set to +true+, header
  #                                       rows are returned in a CSV::Row object
  #                                       with identical headers and
  #                                       fields (save that the fields do not go
  #                                       through the converters).
  # <b><tt>:write_headers</tt></b>::      When +true+ and <tt>:headers</tt> is
  #                                       set, a header row will be added to the
  #                                       output.
  # <b><tt>:header_converters</tt></b>::  Identical in functionality to
  #                                       <tt>:converters</tt> save that the
  #                                       conversions are only made to header
  #                                       rows.  All built-in converters try to
  #                                       transcode headers to UTF-8 before
  #                                       converting.  The conversion will fail
  #                                       if the data cannot be transcoded,
  #                                       leaving the header unchanged.
  # <b><tt>:skip_blanks</tt></b>::        When set to a +true+ value, CSV will
  #                                       skip over any rows with no content.
  # <b><tt>:force_quotes</tt></b>::       When set to a +true+ value, CSV will
  #                                       quote all CSV fields it creates.
  #
  # See CSV::DEFAULT_OPTIONS for the default settings.
  #
  # Options cannot be overridden in the instance methods for performance reasons,
  # so be sure to set what you want here.
  #
  def initialize(data, options = Hash.new)
    # build the options for this read/write
    options = DEFAULT_OPTIONS.merge(options)

    # create the IO object we will read from
    @io       = data.is_a?(String) ? StringIO.new(data) : data
    # honor the IO encoding if we can, otherwise default to ASCII-8BIT
    @encoding = raw_encoding(nil) ||
                ( if encoding = options.delete(:internal_encoding)
                    case encoding
                    when Encoding; encoding
                    else Encoding.find(encoding)
                    end
                  end ) ||
                ( case encoding = options.delete(:encoding)
                  when Encoding; encoding
                  when /\A[^:]+/; Encoding.find($&)
                  end ) ||
                Encoding.default_internal || Encoding.default_external
    #
    # prepare for building safe regular expressions in the target encoding,
    # if we can transcode the needed characters
    #
    @re_esc   =   "\\".encode(@encoding) rescue ""
    @re_chars =   /#{%"[-][\\.^$?*+{}()|# \r\n\t\f\v]".encode(@encoding)}/
    # @re_chars =   /#{%"[-][\\.^$?*+{}()|# \r\n\t\f\v]".encode(@encoding, fallback: proc{""})}/

    init_separators(options)
    init_parsers(options)
    init_converters(options)
    init_headers(options)

    options.delete(:encoding)
    options.delete(:internal_encoding)
    options.delete(:external_encoding)
    unless options.empty?
      raise ArgumentError, "Unknown options:  #{options.keys.join(', ')}."
    end

    # track our own lineno since IO gets confused about line-ends is CSV fields
    @lineno = 0
  end

  #
  # The encoded <tt>:col_sep</tt> used in parsing and writing.  See CSV::new
  # for details.
  #
  attr_reader :col_sep
  #
  # The encoded <tt>:row_sep</tt> used in parsing and writing.  See CSV::new
  # for details.
  #
  attr_reader :row_sep
  #
  # The encoded <tt>:quote_char</tt> used in parsing and writing.  See CSV::new
  # for details.
  #
  attr_reader :quote_char
  # The limit for field size, if any.  See CSV::new for details.
  attr_reader :field_size_limit
  #
  # Returns the current list of converters in effect.  See CSV::new for details.
  # Built-in converters will be returned by name, while others will be returned
  # as is.
  #
  def converters
    @converters.map do |converter|
      name = Converters.rassoc(converter)
      name ? name.first : converter
    end
  end
  #
  # Returns +true+ if unconverted_fields() to parsed results.  See CSV::new
  # for details.
  #
  def unconverted_fields?() @unconverted_fields end
  #
  # Returns +nil+ if headers will not be used, +true+ if they will but have not
  # yet been read, or the actual headers after they have been read.  See
  # CSV::new for details.
  #
  def headers
    @headers || true if @use_headers
  end
  #
  # Returns +true+ if headers will be returned as a row of results.
  # See CSV::new for details.
  #
  def return_headers?()     @return_headers     end
  # Returns +true+ if headers are written in output. See CSV::new for details.
  def write_headers?()      @write_headers      end
  #
  # Returns the current list of converters in effect for headers.  See CSV::new
  # for details.  Built-in converters will be returned by name, while others
  # will be returned as is.
  #
  def header_converters
    @header_converters.map do |converter|
      name = HeaderConverters.rassoc(converter)
      name ? name.first : converter
    end
  end
  #
  # Returns +true+ blank lines are skipped by the parser. See CSV::new
  # for details.
  #
  def skip_blanks?()        @skip_blanks        end
  # Returns +true+ if all output fields are quoted. See CSV::new for details.
  def force_quotes?()       @force_quotes       end

  #
  # The Encoding CSV is parsing or writing in.  This will be the Encoding you
  # receive parsed data in and/or the Encoding data will be written in.
  #
  attr_reader :encoding

  #
  # The line number of the last row read from this file.  Fields with nested
  # line-end characters will not affect this count.
  #
  attr_reader :lineno

  ### IO and StringIO Delegation ###

  extend Forwardable
  def_delegators :@io, :binmode, :binmode?, :close, :close_read, :close_write,
                       :closed?, :eof, :eof?, :external_encoding, :fcntl,
                       :fileno, :flock, :flush, :fsync, :internal_encoding,
                       :ioctl, :isatty, :path, :pid, :pos, :pos=, :reopen,
                       :seek, :stat, :string, :sync, :sync=, :tell, :to_i,
                       :to_io, :truncate, :tty?

  # Rewinds the underlying IO object and resets CSV's lineno() counter.
  def rewind
    @headers = nil
    @lineno  = 0

    @io.rewind
  end

  ### End Delegation ###

  #
  # The primary write method for wrapped Strings and IOs, +row+ (an Array or
  # CSV::Row) is converted to CSV and appended to the data source.  When a
  # CSV::Row is passed, only the row's fields() are appended to the output.
  #
  # The data source must be open for writing.
  #
  def <<(row)
    # make sure headers have been assigned
    if header_row? and [Array, String].include? @use_headers.class
      parse_headers  # won't read data for Array or String
      self << @headers if @write_headers
    end

    # handle CSV::Row objects and Hashes
    row = case row
          when self.class::Row then row.fields
          when Hash            then @headers.map { |header| row[header] }
          else                      row
          end

    @headers =  row if header_row?
    @lineno  += 1

    output = row.map(&@quote).join(@col_sep) + @row_sep  # quote and separate
    if @io.is_a?(StringIO)             and
       output.encoding != raw_encoding and
       (compatible_encoding = Encoding.compatible?(@io.string, output))
      @io = StringIO.new(@io.string.force_encoding(compatible_encoding))
      @io.seek(0, IO::SEEK_END)
    end
    @io << output

    self  # for chaining
  end
  alias_method :add_row, :<<
  alias_method :puts,    :<<

  #
  # :call-seq:
  #   convert( name )
  #   convert { |field| ... }
  #   convert { |field, field_info| ... }
  #
  # You can use this method to install a CSV::Converters built-in, or provide a
  # block that handles a custom conversion.
  #
  # If you provide a block that takes one argument, it will be passed the field
  # and is expected to return the converted value or the field itself.  If your
  # block takes two arguments, it will also be passed a CSV::FieldInfo Struct,
  # containing details about the field.  Again, the block should return a
  # converted field or the field itself.
  #
  def convert(name = nil, &converter)
    add_converter(:converters, self.class::Converters, name, &converter)
  end

  #
  # :call-seq:
  #   header_convert( name )
  #   header_convert { |field| ... }
  #   header_convert { |field, field_info| ... }
  #
  # Identical to CSV#convert(), but for header rows.
  #
  # Note that this method must be called before header rows are read to have any
  # effect.
  #
  def header_convert(name = nil, &converter)
    add_converter( :header_converters,
                   self.class::HeaderConverters,
                   name,
                   &converter )
  end

  include Enumerable

  #
  # Yields each row of the data source in turn.
  #
  # Support for Enumerable.
  #
  # The data source must be open for reading.
  #
  def each
    if block_given?
      while row = shift
        yield row
      end
    else
      to_enum
    end
  end

  #
  # Slurps the remaining rows and returns an Array of Arrays.
  #
  # The data source must be open for reading.
  #
  def read
    rows = to_a
    if @use_headers
      Table.new(rows)
    else
      rows
    end
  end
  alias_method :readlines, :read

  # Returns +true+ if the next row read will be a header row.
  def header_row?
    @use_headers and @headers.nil?
  end

  #
  # The primary read method for wrapped Strings and IOs, a single row is pulled
  # from the data source, parsed and returned as an Array of fields (if header
  # rows are not used) or a CSV::Row (when header rows are used).
  #
  # The data source must be open for reading.
  #
  def shift
    #########################################################################
    ### This method is purposefully kept a bit long as simple conditional ###
    ### checks are faster than numerous (expensive) method calls.         ###
    #########################################################################

    # handle headers not based on document content
    if header_row? and @return_headers and
       [Array, String].include? @use_headers.class
      if @unconverted_fields
        return add_unconverted_fields(parse_headers, Array.new)
      else
        return parse_headers
      end
    end

    #
    # it can take multiple calls to <tt>@io.gets()</tt> to get a full line,
    # because of \r and/or \n characters embedded in quoted fields
    #
    in_extended_col = false
    csv             = Array.new

    loop do
      # add another read to the line
      unless parse = @io.gets(@row_sep)
        return nil
      end

      parse.sub!(@parsers[:line_end], "")

      if csv.empty?
        #
        # I believe a blank line should be an <tt>Array.new</tt>, not Ruby 1.8
        # CSV's <tt>[nil]</tt>
        #
        if parse.empty?
          @lineno += 1
          if @skip_blanks
            next
          elsif @unconverted_fields
            return add_unconverted_fields(Array.new, Array.new)
          elsif @use_headers
            return self.class::Row.new(Array.new, Array.new)
          else
            return Array.new
          end
        end
      end

      parts =  parse.split(@col_sep, -1)
      if parts.empty?
        if in_extended_col
          csv[-1] << @col_sep   # will be replaced with a @row_sep after the parts.each loop
        else
          csv << nil
        end
      end

      # This loop is the hot path of csv parsing. Some things may be non-dry
      # for a reason. Make sure to benchmark when refactoring.
      parts.each do |part|
        if in_extended_col
          # If we are continuing a previous column
          if part[-1] == @quote_char && part.count(@quote_char) % 2 != 0
            # extended column ends
            csv.last << part[0..-2]
            if csv.last =~ @parsers[:stray_quote]
              raise MalformedCSVError,
                    "Missing or stray quote in line #{lineno + 1}"
            end
            csv.last.gsub!(@quote_char * 2, @quote_char)
            in_extended_col = false
          else
            csv.last << part
            csv.last << @col_sep
          end
        elsif part[0] == @quote_char
          # If we are staring a new quoted column
          if part[-1] != @quote_char || part.count(@quote_char) % 2 != 0
            # start an extended column
            csv             << part[1..-1]
            csv.last        << @col_sep
            in_extended_col =  true
          else
            # regular quoted column
            csv << part[1..-2]
            if csv.last =~ @parsers[:stray_quote]
              raise MalformedCSVError,
                    "Missing or stray quote in line #{lineno + 1}"
            end
            csv.last.gsub!(@quote_char * 2, @quote_char)
          end
        elsif part =~ @parsers[:quote_or_nl]
          # Unquoted field with bad characters.
          if part =~ @parsers[:nl_or_lf]
            raise MalformedCSVError, "Unquoted fields do not allow " +
                                     "\\r or \\n (line #{lineno + 1})."
          else
            raise MalformedCSVError, "Illegal quoting in line #{lineno + 1}."
          end
        else
          # Regular ole unquoted field.
          csv << (part.empty? ? nil : part)
        end
      end

      # Replace tacked on @col_sep with @row_sep if we are still in an extended
      # column.
      csv[-1][-1] = @row_sep if in_extended_col

      if in_extended_col
        # if we're at eof?(), a quoted field wasn't closed...
        if @io.eof?
          raise MalformedCSVError,
                "Unclosed quoted field on line #{lineno + 1}."
        elsif @field_size_limit and csv.last.size >= @field_size_limit
          raise MalformedCSVError, "Field size exceeded on line #{lineno + 1}."
        end
        # otherwise, we need to loop and pull some more data to complete the row
      else
        @lineno += 1

        # save fields unconverted fields, if needed...
        unconverted = csv.dup if @unconverted_fields

        # convert fields, if needed...
        csv = convert_fields(csv) unless @use_headers or @converters.empty?
        # parse out header rows and handle CSV::Row conversions...
        csv = parse_headers(csv)  if     @use_headers

        # inject unconverted fields and accessor, if requested...
        if @unconverted_fields and not csv.respond_to? :unconverted_fields
          add_unconverted_fields(csv, unconverted)
        end

        # return the results
        break csv
      end
    end
  end
  alias_method :gets,     :shift
  alias_method :readline, :shift

  #
  # Returns a simplified description of the key CSV attributes in an
  # ASCII compatible String.
  #
  def inspect
    str = ["<#", self.class.to_s, " io_type:"]
    # show type of wrapped IO
    if    @io == $stdout then str << "$stdout"
    elsif @io == $stdin  then str << "$stdin"
    elsif @io == $stderr then str << "$stderr"
    else                      str << @io.class.to_s
    end
    # show IO.path(), if available
    if @io.respond_to?(:path) and (p = @io.path)
      str << " io_path:" << p.inspect
    end
    # show encoding
    str << " encoding:" << @encoding.name
    # show other attributes
    %w[ lineno     col_sep     row_sep
        quote_char skip_blanks ].each do |attr_name|
      if a = instance_variable_get("@#{attr_name}")
        str << " " << attr_name << ":" << a.inspect
      end
    end
    if @use_headers
      str << " headers:" << headers.inspect
    end
    str << ">"
    begin
      str.join('')
    rescue  # any encoding error
      str.map do |s|
        e = Encoding::Converter.asciicompat_encoding(s.encoding)
        e ? s.encode(e) : s.force_encoding("ASCII-8BIT")
      end.join('')
    end
  end

  private

  #
  # Stores the indicated separators for later use.
  #
  # If auto-discovery was requested for <tt>@row_sep</tt>, this method will read
  # ahead in the <tt>@io</tt> and try to find one.  +ARGF+, +STDIN+, +STDOUT+,
  # +STDERR+ and any stream open for output only with a default
  # <tt>@row_sep</tt> of <tt>$INPUT_RECORD_SEPARATOR</tt> (<tt>$/</tt>).
  #
  # This method also establishes the quoting rules used for CSV output.
  #
  def init_separators(options)
    # store the selected separators
    @col_sep    = options.delete(:col_sep).to_s.encode(@encoding)
    @row_sep    = options.delete(:row_sep)  # encode after resolving :auto
    @quote_char = options.delete(:quote_char).to_s.encode(@encoding)

    if @quote_char.length != 1
      raise ArgumentError, ":quote_char has to be a single character String"
    end

    #
    # automatically discover row separator when requested
    # (not fully encoding safe)
    #
    if @row_sep == :auto
      if [ARGF, STDIN, STDOUT, STDERR].include?(@io) or
         (defined?(Zlib) and @io.class == Zlib::GzipWriter)
        @row_sep = $INPUT_RECORD_SEPARATOR
      else
        begin
          #
          # remember where we were (pos() will raise an axception if @io is pipe
          # or not opened for reading)
          #
          saved_pos = @io.pos
          while @row_sep == :auto
            #
            # if we run out of data, it's probably a single line
            # (ensure will set default value)
            #
            break unless sample = @io.gets(nil, 1024)
            # extend sample if we're unsure of the line ending
            if sample.end_with? encode_str("\r")
              sample << (@io.gets(nil, 1) || "")
            end

            # try to find a standard separator
            if sample =~ encode_re("\r\n?|\n")
              @row_sep = $&
              break
            end
          end

          # tricky seek() clone to work around GzipReader's lack of seek()
          @io.rewind
          # reset back to the remembered position
          while saved_pos > 1024  # avoid loading a lot of data into memory
            @io.read(1024)
            saved_pos -= 1024
          end
          @io.read(saved_pos) if saved_pos.nonzero?
        rescue IOError         # not opened for reading
          # do nothing:  ensure will set default
        rescue NoMethodError   # Zlib::GzipWriter doesn't have some IO methods
          # do nothing:  ensure will set default
        rescue SystemCallError # pipe
          # do nothing:  ensure will set default
        ensure
          #
          # set default if we failed to detect
          # (stream not opened for reading, a pipe, or a single line of data)
          #
          @row_sep = $INPUT_RECORD_SEPARATOR if @row_sep == :auto
        end
      end
    end
    @row_sep = @row_sep.to_s.encode(@encoding)

    # establish quoting rules
    @force_quotes   = options.delete(:force_quotes)
    do_quote        = lambda do |field|
      field         = String(field)
      encoded_quote = @quote_char.encode(field.encoding)
      encoded_quote                                +
      field.gsub(encoded_quote, encoded_quote * 2) +
      encoded_quote
    end
    quotable_chars = encode_str("\r\n", @col_sep, @quote_char)
    @quote         = if @force_quotes
      do_quote
    else
      lambda do |field|
        if field.nil?  # represent +nil+ fields as empty unquoted fields
          ""
        else
          field = String(field)  # Stringify fields
          # represent empty fields as empty quoted fields
          if field.empty? or
             field.count(quotable_chars).nonzero?
            do_quote.call(field)
          else
            field  # unquoted field
          end
        end
      end
    end
  end

  # Pre-compiles parsers and stores them by name for access during reads.
  def init_parsers(options)
    # store the parser behaviors
    @skip_blanks      = options.delete(:skip_blanks)
    @field_size_limit = options.delete(:field_size_limit)

    # prebuild Regexps for faster parsing
    esc_row_sep = escape_re(@row_sep)
    esc_quote   = escape_re(@quote_char)
    @parsers = {
      # for detecting parse errors
      quote_or_nl:    encode_re("[", esc_quote, "\r\n]"),
      nl_or_lf:       encode_re("[\r\n]"),
      stray_quote:    encode_re( "[^", esc_quote, "]", esc_quote,
                                 "[^", esc_quote, "]" ),
      # safer than chomp!()
      line_end:       encode_re(esc_row_sep, "\\z"),
      # illegal unquoted characters
      return_newline: encode_str("\r\n")
    }
  end

  #
  # Loads any converters requested during construction.
  #
  # If +field_name+ is set <tt>:converters</tt> (the default) field converters
  # are set.  When +field_name+ is <tt>:header_converters</tt> header converters
  # are added instead.
  #
  # The <tt>:unconverted_fields</tt> option is also actived for
  # <tt>:converters</tt> calls, if requested.
  #
  def init_converters(options, field_name = :converters)
    if field_name == :converters
      @unconverted_fields = options.delete(:unconverted_fields)
    end

    instance_variable_set("@#{field_name}", Array.new)

    # find the correct method to add the converters
    convert = method(field_name.to_s.sub(/ers\Z/, ""))

    # load converters
    unless options[field_name].nil?
      # allow a single converter not wrapped in an Array
      unless options[field_name].is_a? Array
        options[field_name] = [options[field_name]]
      end
      # load each converter...
      options[field_name].each do |converter|
        if converter.is_a? Proc  # custom code block
          convert.call(&converter)
        else                     # by name
          convert.call(converter)
        end
      end
    end

    options.delete(field_name)
  end

  # Stores header row settings and loads header converters, if needed.
  def init_headers(options)
    @use_headers    = options.delete(:headers)
    @return_headers = options.delete(:return_headers)
    @write_headers  = options.delete(:write_headers)

    # headers must be delayed until shift(), in case they need a row of content
    @headers = nil

    init_converters(options, :header_converters)
  end

  #
  # The actual work method for adding converters, used by both CSV.convert() and
  # CSV.header_convert().
  #
  # This method requires the +var_name+ of the instance variable to place the
  # converters in, the +const+ Hash to lookup named converters in, and the
  # normal parameters of the CSV.convert() and CSV.header_convert() methods.
  #
  def add_converter(var_name, const, name = nil, &converter)
    if name.nil?  # custom converter
      instance_variable_get("@#{var_name}") << converter
    else          # named converter
      combo = const[name]
      case combo
      when Array  # combo converter
        combo.each do |converter_name|
          add_converter(var_name, const, converter_name)
        end
      else        # individual named converter
        instance_variable_get("@#{var_name}") << combo
      end
    end
  end

  #
  # Processes +fields+ with <tt>@converters</tt>, or <tt>@header_converters</tt>
  # if +headers+ is passed as +true+, returning the converted field set.  Any
  # converter that changes the field into something other than a String halts
  # the pipeline of conversion for that field.  This is primarily an efficiency
  # shortcut.
  #
  def convert_fields(fields, headers = false)
    # see if we are converting headers or fields
    converters = headers ? @header_converters : @converters

    fields.map.with_index do |field, index|
      converters.each do |converter|
        field = if converter.arity == 1  # straight field converter
          converter[field]
        else                             # FieldInfo converter
          header = @use_headers && !headers ? @headers[index] : nil
          converter[field, FieldInfo.new(index, lineno, header)]
        end
        break unless field.is_a? String  # short-curcuit pipeline for speed
      end
      field  # final state of each field, converted or original
    end
  end

  #
  # This method is used to turn a finished +row+ into a CSV::Row.  Header rows
  # are also dealt with here, either by returning a CSV::Row with identical
  # headers and fields (save that the fields do not go through the converters)
  # or by reading past them to return a field row. Headers are also saved in
  # <tt>@headers</tt> for use in future rows.
  #
  # When +nil+, +row+ is assumed to be a header row not based on an actual row
  # of the stream.
  #
  def parse_headers(row = nil)
    if @headers.nil?                # header row
      @headers = case @use_headers  # save headers
                 # Array of headers
                 when Array then @use_headers
                 # CSV header String
                 when String
                   self.class.parse_line( @use_headers,
                                          col_sep:    @col_sep,
                                          row_sep:    @row_sep,
                                          quote_char: @quote_char )
                 # first row is headers
                 else            row
                 end

      # prepare converted and unconverted copies
      row      = @headers                       if row.nil?
      @headers = convert_fields(@headers, true)

      if @return_headers                                     # return headers
        return self.class::Row.new(@headers, row, true)
      elsif not [Array, String].include? @use_headers.class  # skip to field row
        return shift
      end
    end

    self.class::Row.new(@headers, convert_fields(row))  # field row
  end

  #
  # This method injects an instance variable <tt>unconverted_fields</tt> into
  # +row+ and an accessor method for +row+ called unconverted_fields().  The
  # variable is set to the contents of +fields+.
  #
  def add_unconverted_fields(row, fields)
    class << row
      attr_reader :unconverted_fields
    end
    row.instance_eval { @unconverted_fields = fields }
    row
  end

  #
  # This method is an encoding safe version of Regexp::escape().  It will escape
  # any characters that would change the meaning of a regular expression in the
  # encoding of +str+.  Regular expression characters that cannot be transcoded
  # to the target encoding will be skipped and no escaping will be performed if
  # a backslash cannot be transcoded.
  #
  def escape_re(str)
    str.gsub(@re_chars) {|c| @re_esc + c}
  end

  #
  # Builds a regular expression in <tt>@encoding</tt>.  All +chunks+ will be
  # transcoded to that encoding.
  #
  def encode_re(*chunks)
    Regexp.new(encode_str(*chunks))
  end

  #
  # Builds a String in <tt>@encoding</tt>.  All +chunks+ will be transcoded to
  # that encoding.
  #
  def encode_str(*chunks)
    chunks.map { |chunk| chunk.encode(@encoding.name) }.join('')
  end

  private

  # 
  # Returns the encoding of the internal IO object or the +default+ if the 
  # encoding cannot be determined.
  #
  def raw_encoding(default = Encoding::ASCII_8BIT)
    if @io.respond_to? :internal_encoding
      @io.internal_encoding || @io.external_encoding
    elsif @io.is_a? StringIO
      @io.string.encoding
    elsif @io.respond_to? :encoding
      @io.encoding
    else
      default
    end
  end
end

# Another name for CSV::instance().
def CSV(*args, &block)
  CSV.instance(*args, &block)
end

class Array
  # Equivalent to <tt>CSV::generate_line(self, options)</tt>.
  def to_csv(options = Hash.new)
    CSV.generate_line(self, options)
  end
end

class String
  # Equivalent to <tt>CSV::parse_line(self, options)</tt>.
  def parse_csv(options = Hash.new)
    CSV.parse_line(self, options)
  end
end
