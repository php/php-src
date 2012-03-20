# tree.rb --
#
# This demonstration script creates a toplevel window containing a Ttk
# tree widget.
#
# based on "Id: tree.tcl,v 1.4 2007/12/13 15:27:07 dgp Exp"

if defined?($tree_demo) && $tree_demo
  $tree_demo.destroy
  $tree_demo = nil
end

$tree_demo = TkToplevel.new {|w|
  title("Directory Browser")
  iconname("tree")
  positionWindow(w)
}

base_frame = TkFrame.new($tree_demo).pack(:fill=>:both, :expand=>true)

## Explanatory text
Ttk::Label.new(base_frame, :font=>$font, :wraplength=>'4i',
               :justify=>:left, :anchor=>'n', :padding=>[10, 2, 10, 6],
               :text=><<EOL).pack(:fill=>:x)
Ttk is the new Tk themed widget set. \
One of the widgets it includes is a tree widget, \
which allows the user to browse a hierarchical data-set such as a filesystem. \
The tree widget not only allows for the tree part itself, \
but it also supports an arbitrary number of additional columns \
which can show additional data (in this case, the size of the files \
found in your filesystem). \
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
                         :command=>proc{showCode 'tree'}),
         Ttk::Button.new(frame, :text=>'Dismiss',
                         :image=>$image['delete'], :compound=>:left,
                         :command=>proc{
                           $tree_demo.destroy
                           $tree_demo = nil
                         }),
         :padx=>4, :pady=>4)
  grid_columnconfigure(0, :weight=>1)
  pack(:side=>:bottom, :fill=>:x)
}

## Code to populate the roots of the tree (can be more than one on Windows)
def populate_roots(tree)
  TkComm.simplelist(Tk.tk_call('file', 'volumes')).sort.each{|dir|
    populate_tree(tree, tree.insert(nil, :end, :text=>dir,
                                    :values=>[dir, 'directory']))
  }
end

## Code to populate a node of the tree
def populate_tree(tree, node)
  return if tree.get(node, :type) != 'directory'

  path = tree.get(node, :fullpath)
  tree.delete(tree.children(node))
  Dir.glob("#{path}/*").sort.each{|f|
    type = File.ftype(f)
    id = tree.insert(node, :end,
                     :text=>File.basename(f), :values=>[f, type]).id
    if type == 'directory'
      ## Make it so that this node is openable
      tree.insert(id, 0, :text=>'dummy')
      tree.itemconfigure(id, :text=>File.basename(f))
    elsif type == 'file'
      size = File.size(f)
      if size >= 1024*1024*1024
        size = '%.1f GB' % (size.to_f/1024/1024/1024)
      elsif size >= 1024*1024
        size = '%.1f MB' % (size.to_f/1024/1024)
      elsif size >= 1024
        size = '%.1f KB' % (size.to_f/1024)
      else
        size = '%.1f bytes' % (size.to_f/1024)
      end
      tree.set(id, :size, size)
    end
  }

  # Stop this code from rerunning on the current node
  tree.set(node, :type, 'processed_directory')
end

## Create the tree and set it up
tree = Ttk::Treeview.new(base_frame, :columns=>%w(fullpath type size),
                         :displaycolumns=>['size'])
if Tk.windowingsystem != 'aqua'
  vsb = tree.yscrollbar(Ttk::Scrollbar.new(base_frame))
  hsb = tree.xscrollbar(Ttk::Scrollbar.new(base_frame))
else
  vsb = tree.yscrollbar(Tk::Scrollbar.new(base_frame))
  hsb = tree.xscrollbar(Tk::Scrollbar.new(base_frame))
end

tree.heading_configure('#0', :text=>'Directory Structure')
tree.heading_configure('size', :text=>'File Size')
tree.column_configure('size', :stretch=>0, :width=>70)
populate_roots(tree)
tree.bind('<TreeviewOpen>', '%W'){|w| populate_tree(w, w.focus_item)}

## Arrange the tree and its scrollbars in the toplevel
container = Ttk::Frame.new(base_frame).pack(:fill=>:both, :expand=>true)
container.lower
Tk.grid(tree, vsb, :in=>container, :sticky=>'nsew')
Tk.grid(hsb,       :in=>container, :sticky=>'nsew')
container.grid_columnconfigure(0, :weight=>1)
container.grid_rowconfigure(0, :weight=>1)
