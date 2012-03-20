#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

class Spinner_demo < TkWindow
  Months = %w(January February March April May June July August September October November December)

  def block_input(c)
    false
  end

  def spin_month(step)
    index = Months.index(@spinner.get) + step
    index = 11 if index < 0
    index = 0 if index > 11

    @spinner.value = Months[index]
  end

  def initialize(parent=nil)
    @spinner = Tk::Iwidgets::Spinner.new(parent, :labeltext=>'Month : ',
                                         :width=>10, :fixed=>10,
                                         :validate=>proc{|c| block_input},
                                         :decrement=>proc{spin_month -1},
                                         :increment=>proc{spin_month 1})
    @path = @spinner
    @spinner.insert(0, Months[0])
  end
end

Spinner_demo.new.pack(:padx=>10, :pady=>10)

Tk.mainloop
