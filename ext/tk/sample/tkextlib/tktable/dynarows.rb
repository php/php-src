#!/usr/bin/env ruby
##
## dynarows.rb
##
## This demos shows the use of the validation mechanism of the table
## and uses the table's cache (no -command or -variable) with a cute
## dynamic row routine.
##
## ( based on 'dynarows.tcl' included source archive of tktable extension )
##
require 'tk'
require 'tkextlib/tktable'

def table_validate(w, idx)
  return unless idx =~ /^(\d+),(\d+)$/
  row = Integer($1)
  col = Integer($2)
  val = w.get(idx)

 [w, idx]
  nrows = w[:rows]
  return if row == nrows - 1 && val == ''

  begin
    time = Tk.tk_call('clock', 'scan', val)
    date = []
    Tk.tk_call('clock', 'format', time,
               :format=>'%m %d %Y').split(' ').each{|item|
      date << item.sub(/^\s*0*/,'')
    }
    w.set(idx, date.join('/'))
    if row == nrows - 1
      if w.get([row,1]) != '' && w.get([row,2]) != ''
        w.tag_row_reset(row)
        w.set([row,0], row)
        nrows += 1
        row += 1
        w.configure(:rows=>nrows)
        w.tag_row('unset', row)
        w.set([row,0], '*')
        w.see([row,1])
        w.activate([row,1])
      end
    end
  rescue
    Tk.bell
    w.activate(idx)
    w.selection_clear_all
    w.selection_set(:active)
    w.see(:active)
  end
end


lbl = TkLabel.new(:text=>"Dynamic Date Validated Rows")

table = Tk::TkTable.new(:rows=>2, :cols=>3, :cache=>1, :selecttype=>:row,
                        :titlerows=>1, :titlecols=>1, :height=>5,
                        :colstretch=>:unset, :rowstretch=>:unset,
                        :autoclear=>true,
                        :browsecommand=>[
                          proc{|w,s| table_validate(w, s)},
                          '%W %s'
                        ])
table.set([0,1], 'Begin', [0,2], 'End', [1,0], '*')
table.tag_configure('unset', :fg=>'#008811')
table.tag_configure('title', :fg=>'red')
table.tag_row('unset', 1)
table.set_width(0,3)

sx = table.xscrollbar(TkScrollbar.new)
sy = table.yscrollbar(TkScrollbar.new)

Tk.grid(lbl, '-', :sticky=>:ew)
Tk.grid(table, sy, :sticky=>:news)
Tk.grid(sx, :sticky=>:ew)

Tk.root.grid_columnconfig(0, :weight=>1)
Tk.root.grid_rowconfig(1, :weight=>1)

rtn_proc = proc{|w|
  r = w.row_index(:active)
  c = w.col_index(:active)

  if c == 2
    r += 1
    w.activate([r,1])
  else
    c += 1
    w.activate([r,c])
  end
  w.see(:active)
  Tk.callback_break
}

table.bind('Return', rtn_proc, '%W')
table.bind('KP_Enter', rtn_proc, '%W')

Tk.mainloop
