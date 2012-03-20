# mclist.rb --
#
# This demonstration script creates a toplevel window containing a Ttk
# tree widget configured as a multi-column listbox.
#
# based on "Id: mclist.tcl,v 1.3 2007/12/13 15:27:07 dgp Exp"

if defined?($mclist_demo) && $mclist_demo
  $mclist_demo.destroy
  $mclist_demo = nil
end

$mclist_demo = TkToplevel.new {|w|
  title("Multi-Column List")
  iconname("mclist")
  positionWindow(w)
}

base_frame = TkFrame.new($mclist_demo).pack(:fill=>:both, :expand=>true)

## Explanatory text
Ttk::Label.new(base_frame, :font=>$font, :wraplength=>'4i',
               :justify=>:left, :anchor=>'n', :padding=>[10, 2, 10, 6],
               :text=><<EOL).pack(:fill=>:x)
Ttk is the new Tk themed widget set. \
One of the widgets it includes is a tree widget, \
which can be configured to display multiple columns of informational data \
without displaying the tree itself. \
This is a simple way to build a listbox that has multiple columns. \
Clicking on the heading for a column will sort the data by that column. \
You can also change the width of the columns \
by dragging the boundary between them.
EOL

## See Code / Dismiss
Ttk::Frame.new(base_frame) {|frame|
  sep = Ttk::Separator.new(frame)
  Tk.grid(sep, :columnspan=>4, :row=>0, :sticky=>'ew', :pady=>2)
  TkGrid('x',
         Ttk::Button.new(frame, :text=>'See Code',
                         :image=>$image['view'], :compound=>:left,
                         :command=>proc{showCode 'mclist'}),
         Ttk::Button.new(frame, :text=>'Dismiss',
                         :image=>$image['delete'], :compound=>:left,
                         :command=>proc{
                           $mclist_demo.destroy
                           $mclist_demo = nil
                         }),
         :padx=>4, :pady=>4)
  grid_columnconfigure(0, :weight=>1)
  pack(:side=>:bottom, :fill=>:x)
}

container = Ttk::Frame.new(base_frame)
tree = Ttk::Treeview.new(base_frame, :columns=>%w(country capital currency),
                          :show=>:headings)
if Tk.windowingsystem != 'aqua'
  vsb = tree.yscrollbar(Ttk::Scrollbar.new(base_frame))
  hsb = tree.xscrollbar(Ttk::Scrollbar.new(base_frame))
else
  vsb = tree.yscrollbar(Tk::Scrollbar.new(base_frame))
  hsb = tree.xscrollbar(Tk::Scrollbar.new(base_frame))
end

container.pack(:fill=>:both, :expand=>true)
Tk.grid(tree, vsb, :in=>container, :sticky=>'nsew')
Tk.grid(hsb,       :in=>container, :sticky=>'nsew')
container.grid_columnconfigure(0, :weight=>1)
container.grid_rowconfigure(0, :weight=>1)

## The data we're going to insert
data = [
  ['Argentina', 	'Buenos Aires', 	'ARS'],
  ['Australia',		'Canberra',		'AUD'],
  ['Brazil', 		'Brazilia', 		'BRL'],
  ['Canada', 		'Ottawa', 		'CAD'],
  ['China',		'Beijing', 		'CNY'],
  ['France',		'Paris', 		'EUR'],
  ['Germany', 		'Berlin',		'EUR'],
  ['India', 		'New Delhi',		'INR'],
  ['Italy', 		'Rome', 		'EUR'],
  ['Japan', 		'Tokyo', 		'JPY'],
  ['Mexico', 		'Mexico City', 		'MXN'],
  ['Russia', 		'Moscow', 		'RUB'],
  ['South Africa',	'Pretoria', 		'ZAR'],
  ['United Kingdom', 	'London', 		'GBP'],
  ['United States', 	'Washington, D.C.', 	'USD'],
]

## Code to insert the data nicely
font = Ttk::Style.lookup(tree[:style], :font)
cols = %w(country capital currency)
cols.zip(%w(Country Capital Currency)).each{|col, name|
  tree.heading_configure(col, :text=>name,
                         :command=>proc{sort_by(tree, col, false)})
  tree.column_configure(col, :width=>TkFont.measure(font, name))
}

data.each{|country, capital, currency|
  #tree.insert('', :end, :values=>[country, capital, currency])
  tree.insert(nil, :end, :values=>[country, capital, currency])
  cols.zip([country, capital, currency]).each{|col, val|
    len = TkFont.measure(font, "#{val}  ")
    if tree.column_cget(col, :width) < len
      tree.column_configure(col, :width=>len)
    end
  }
}

## Code to do the sorting of the tree contents when clicked on
def sort_by(tree, col, direction)
  tree.children(nil).map!{|row| [tree.get(row, col), row.id]} .
    sort(&((direction)? proc{|x, y| y <=> x}: proc{|x, y| x <=> y})) .
    each_with_index{|info, idx| tree.move(info[1], nil, idx)}

  tree.heading_configure(col, :command=>proc{sort_by(tree, col, ! direction)})
end
