#!/usr/bin/ruby

require 'tk'
require 'tkextlib/tcllib/datefield'
require 'parsedate'

Tk.root.title('Datefield example')


my_date1 = TkVariable.new
my_date2 = TkVariable.new
my_date1.trace('w'){
  begin
    t = Time.local(*(ParseDate.parsedate(my_date1.value)))
    my_date2.value = t.strftime('%A')
  rescue
    # ignore error
  end
}

df = Tk::Tcllib::Datefield.new(:textvariable=>my_date1)
Tk.grid(TkLabel.new(:text=>'Enter a date:', :anchor=>:e), df, :sticky=>:ew)
Tk.grid(TkLabel.new(:text=>'That date ia a:', :anchor=>:e),
        TkLabel.new(:textvariable=>my_date2, :relief=>:sunken, :width=>12),
        :sticky=>:ew)

df.set_focus

Tk.mainloop
