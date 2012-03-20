=begin
= xmlrpc/datetime.rb
Copyright (C) 2001, 2002, 2003 by Michael Neumann (mneumann@ntecs.de)

Released under the same term of license as Ruby.

= Classes
* ((<XMLRPC::DateTime>))

= XMLRPC::DateTime
== Description
This class is important to handle XMLRPC (('dateTime.iso8601')) values,
correcly, because normal UNIX-dates (class (({Date}))) only handle dates
from year 1970 on, and class (({Time})) handles dates without the time
component. (({XMLRPC::DateTime})) is able to store a XMLRPC
(('dateTime.iso8601')) value correctly.

== Class Methods
--- XMLRPC::DateTime.new( year, month, day, hour, min, sec )
    Creates a new (({XMLRPC::DateTime})) instance with the
    parameters ((|year|)), ((|month|)), ((|day|)) as date and
    ((|hour|)), ((|min|)), ((|sec|)) as time.
    Raises (({ArgumentError})) if a parameter is out of range, or ((|year|)) is not
    of type (({Integer})).

== Instance Methods
--- XMLRPC::DateTime#year
--- XMLRPC::DateTime#month
--- XMLRPC::DateTime#day
--- XMLRPC::DateTime#hour
--- XMLRPC::DateTime#min
--- XMLRPC::DateTime#sec
    Return the value of the specified date/time component.

--- XMLRPC::DateTime#mon
    Alias for ((<XMLRPC::DateTime#month>)).

--- XMLRPC::DateTime#year=( value )
--- XMLRPC::DateTime#month=( value )
--- XMLRPC::DateTime#day=( value )
--- XMLRPC::DateTime#hour=( value )
--- XMLRPC::DateTime#min=( value )
--- XMLRPC::DateTime#sec=( value )
    Set ((|value|)) as the new date/time component.
    Raises (({ArgumentError})) if ((|value|)) is out of range, or in the case
    of (({XMLRPC::DateTime#year=})) if ((|value|)) is not of type (({Integer})).

--- XMLRPC::DateTime#mon=( value )
    Alias for ((<XMLRPC::DateTime#month=>)).

--- XMLRPC::DateTime#to_time
    Return a (({Time})) object of the date/time which (({self})) represents.
    If the (('year')) is below 1970, this method returns (({nil})),
    because (({Time})) cannot handle years below 1970.
    The used timezone is GMT.

--- XMLRPC::DateTime#to_date
    Return a (({Date})) object of the date which (({self})) represents.
    The (({Date})) object do ((*not*)) contain the time component (only date).

--- XMLRPC::DateTime#to_a
    Returns all date/time components in an array.
    Returns (({[year, month, day, hour, min, sec]})).
=end

require "date"

module XMLRPC

class DateTime

  attr_reader :year, :month, :day, :hour, :min, :sec

  def year= (value)
    raise ArgumentError, "date/time out of range" unless value.is_a? Integer
    @year = value
  end

  def month= (value)
    raise ArgumentError, "date/time out of range" unless (1..12).include? value
    @month = value
  end

  def day= (value)
    raise ArgumentError, "date/time out of range" unless (1..31).include? value
    @day = value
  end

  def hour= (value)
    raise ArgumentError, "date/time out of range" unless (0..24).include? value
    @hour = value
  end

  def min= (value)
    raise ArgumentError, "date/time out of range" unless (0..59).include? value
    @min = value
  end

  def sec= (value)
    raise ArgumentError, "date/time out of range" unless (0..59).include? value
    @sec = value
  end

  alias mon  month
  alias mon= month=


  def initialize(year, month, day, hour, min, sec)
    self.year, self.month, self.day = year, month, day
    self.hour, self.min, self.sec   = hour, min, sec
  end

  def to_time
    if @year >= 1970
      Time.gm(*to_a)
    else
      nil
    end
  end

  def to_date
    Date.new(*to_a[0,3])
  end

  def to_a
    [@year, @month, @day, @hour, @min, @sec]
  end

  def ==(o)
    self.to_a == Array(o) rescue false
  end

end


end # module XMLRPC


=begin
= History
    $Id$
=end
