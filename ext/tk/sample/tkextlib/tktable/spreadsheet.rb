#!/usr/bin/env ruby
##
## spreadsheet.rb
##
## This demos shows how you can simulate a 3D table
## and has other basic features to begin a basic spreadsheet
##
## ( based on 'spreadsheet.tcl' included source archive of tktable extension )
##
require 'tk'
require 'tkextlib/tktable'

rows = 10
cols = 10
cur_var = TkVariable.new
table_list = Hash.new{|hash, key| hash[key] = TkVariable.new_hash}
page = TkVariable.new('AA')
color = Hash.new('pink')
color['AA'] = 'orange'
color['BB'] = 'blue'
color['CC'] = 'green'

def colorize(num)
  num = Integer(num)
  return 'colored' if (num > 0 && num % 2 == 1)
end

def fill_table(tbl_list, page, r=10, c=10)
  ary = tbl_list[page]

  (0...r).each{|i|
    (0...c).each{|j|
      if i!=0 && j!=0
        ary[i,j] = "#{page} #{i},#{j}"
      elsif i!=0
        ary[i,j] = i.to_s
      else
        ary[i,j] = (64+j).chr
      end
    }
  }
end

def changepage(tbl_list, tbl, ent, col, var, elem, op)
  if elem != ''
    page = var[elem]
  else
    page = var.value
  end
  if tbl[:variable] != tbl_list[page].id
    tbl.selection_clear_all
    tbl.variable(tbl_list[page])
    ent.textvariable(tbl_list[page].ref('active'))
    tbl.activate('origin')
    tbl.tag_configure('colored', :bg=>col[page])
    tbl.see('active')
  end
end

lbl = TkLabel.new(:text=>"TkTable v1 Spreadsheet Example")

current = TkLabel.new(:textvariable=>cur_var, :width=>5)
entry = TkEntry.new(:textvariable=>table_list[page.value].ref('active'))
lpage = TkLabel.new(:text=>'PAGE:', :width=>6, :anchor=>:e)
optmenu = TkOptionMenubutton.new(page, *(%w(AA BB CC DD)))

fill_table(table_list, page.value)
fill_table(table_list, 'BB', Integer(rows/2), Integer(cols/2))

table = Tk::TkTable.new(:rows=>rows, :cols=>cols, :width=>5, :height=>5,
                        :variable=>table_list[page.value],
                        :titlerows=>1, :titlecols=>1,
                        :coltagcommand=>proc{|n| colorize(n)},
                        :flashmode=>true, :selectmode=>:extended,
                        :colstretch=>:unset, :rowstretch=>:unset,
                        :browsecommand=>proc{|e| cur_var.value = e.new_index})

page.trace(:w, proc{|var, elem, op|
             changepage(table_list, table, entry, color, var, elem, op)
           })

table.tag_configure('colored', :bg=>color[page.value])
table.tag_configure('title',   :fg=>'red', :relief=>:groove)
table.tag_configure('blue',    :bg=>'blue')
table.tag_configure('green',   :bg=>'green')

table.tag_cell('green', [6,3], [5,7], [4,9])
table.tag_cell('blue', [8,8])
table.tag_row('blue', 7)
table.tag_col('blue', 6, 8)
table.set_width([0, 3], [2, 7])

sx = table.xscrollbar(TkScrollbar.new)
sy = table.yscrollbar(TkScrollbar.new)

btn = TkButton.new(:text=>'Exit', :command=>proc{exit})

Tk.grid(lbl, '-', '-', '-', '-', :sticky=>:ew)
Tk.grid(current, entry, lpage, optmenu, '-', :sticky=>:ew)
Tk.grid(table, '-', '-', '-', sy, :sticky=>:ns)
Tk.grid(sx, '-', '-', '-', :sticky=>:ew)
Tk.grid(btn, '-', '-', '-', '-', :sticky=>:ew)

Tk.root.grid_columnconfig(1, :weight=>1)
Tk.root.grid_rowconfig(2, :weight=>1)

table.grid_configure(:sticky=>:news)

entry.bind('Return', proc{
             r = table.row_index(:active)
             c = table.col_index(:active)
             rmax = table[:rows]
             cmax = table[:cols]

             c += 1
             if c == cmax
               c = table[:titlecols]
               r += 1
               if r == rmax
                 r = table[:titlerows]
               end
             end
             table.activate([r, c])
             table.see('active')
           })

menu = TkMenu.new
m_file = TkMenu.new(menu)
Tk.root.menu(menu)
menu.add(:cascade, :label=>'File', :underline=>0, :menu=>m_file)
m_file.add(:command, :label=>'Fill Array',
           :command=>proc{ fill_table(table_list, page.value) })
m_file.add(:command, :label=>'Quit', :command=>proc{exit})

puts "Table is #{table.path} with array #{(table['variable'])}"

Tk.mainloop
