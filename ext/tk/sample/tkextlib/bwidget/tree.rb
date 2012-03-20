#
#  templdlg demo  ---  called from demo.rb
#
unless Object.const_defined?('DemoVar')
  fail RuntimeError, "This is NOT a stand alone script. This script is called from 'demo.rb'. "
end

module DemoTree
  @@count = 0
  @@dblclick = false
  @@top = nil

  def self.create(nb)
    frame = nb.insert('end', 'demoTree', :text=>'Tree')
    pw = Tk::BWidget::PanedWindow.new(frame, :side=>:top)

    pane = pw.add(:weight=>1)
    title = Tk::BWidget::TitleFrame.new(pane, :text=>'Directory tree')
    sw = Tk::BWidget::ScrolledWindow.new(title.get_frame,
                                         :relief=>:sunken, :borderwidth=>2)
    tree = Tk::BWidget::Tree.new(sw, :relief=>:flat, :borderwidth=>0,
                                 :width=>15, :highlightthickness=>0,
                                 :redraw=>false, :dropenabled=>true,
                                 :dragenabled=>true, :dragevent=>3,
                                 :droptypes=>[
                                   'TREE_NODE', [
                                     :copy, [],
                                     :move, [],
                                     :link, []
                                   ],
                                   'LISTBOX_ITEM', [
                                     :copy, [],
                                     :move, [],
                                     :link, []
                                   ]
                                 ],
                                 :opencmd=>proc{|node|
                                   moddir(1, tree, node)
                                 },
                                 :closecmd=>proc{|node|
                                   moddir(0, tree, node)
                                 })
    sw.set_widget(tree)

    sw.pack(:side=>:top, :expand=>true, :fill=>:both)
    title.pack(:fill=>:both, :expand=>true)

    pane = pw.add(:weight=>2)
    lf = Tk::BWidget::TitleFrame.new(pane, :text=>'Content')
    sw = Tk::BWidget::ScrolledWindow.new(lf.get_frame,
                                         :scrollbar=>:horizontal,
                                         :auto=>:none, :relief=>:sunken,
                                         :borderwidth=>2)

    list = Tk::BWidget::ListBox.new(sw, :relief=>:flat, :borderwidth=>0,
                                    :highlightthickness=>0, :width=>20,
                                    :multicolumn=>true, :redraw=>false,
                                    :dragevent=>3, :dropenabled=>true,
                                    :dragenabled=>true,
                                 :droptypes=>[
                                      'TREE_NODE', [
                                        :copy, [],
                                        :move, [],
                                        :link, []
                                      ],
                                      'LISTBOX_ITEM', [
                                        :copy, [],
                                        :move, [],
                                        :link, []
                                      ]
                                    ])
    sw.set_widget(list)

    Tk.pack(sw, lf, :fill=>:both, :expand=>true)

    pw.pack(:fill=>:both, :expand=>true)

    tree.textbind('ButtonPress-1',
                  proc{|node, ev| select('tree', 1, tree, list, node)})
    tree.textbind('Double-ButtonPress-1',
                  proc{|node, ev| select('tree', 2, tree, list, node)})

    list.textbind('ButtonPress-1',
                  proc{|node, ev| select('list', 1, tree, list, node)})
    list.textbind('Double-ButtonPress-1',
                  proc{|node, ev| select('list', 2, tree, list, node)})

    list.imagebind('Double-ButtonPress-1',
                   proc{|node, ev| select('list', 2, tree, list, node)})

    nb.itemconfigure('demoTree',
                     :createcmd=>proc{|*args| init(tree, list, *args)},
                     :raisecmd=>proc{
                       Tk.root.geometry =~
                         /\d+x\d+([+-]{1,2}\d+)([+-]{1,2}\d+)/
                       global_w = ($1 || 0).to_i
                       global_h = ($2 || 0).to_i
                       if @@top
                         Tk::BWidget.place(@@top, 0, 0, :at,
                            global_w - Tk.root.winfo_screenwidth, global_h)
                         @@top.deiconify
                         Tk.root.bind('Unmap', proc{@@top.withdraw})
                         Tk.root.bind('Map',   proc{@@top.deiconify})
                         Tk.root.bind('Configure', proc{|w|
                                if w == Tk.root
                                  Tk.root.geometry =~
                                    /\d+x\d+([+-]{1,2}\d+)([+-]{1,2}\d+)/
                                  global_w = ($1 || 0).to_i
                                  global_h = ($2 || 0).to_i
                                  Tk::BWidget.place(@@top, 0, 0, :at,
                                       global_w - Tk.root.winfo_screenwidth,
                                       global_h)
                                end
                         }, '%W')
                       end
                     },
                     :leavecmd=>proc{
                       @@top.withdraw if @@top
                       Tk.root.bind_remove('Unmap')
                       Tk.root.bind_remove('Map')
                       Tk.root.bind_remove('Configure')
                       true
                     })
  end

  def self.init(tree, list, *args)
    @@count = 0
    if Tk::PLATFORM['platform'] == 'unix'
      rootdir = File.expand_path('~')
    else
      rootdir = 'c:'
    end

    tree.insert('end', 'root', 'home',
                :text=>rootdir, :data=>rootdir, :open=>true,
                :image=>Tk::BWidget::Bitmap.new('openfold'))
    getdir(tree, 'home', rootdir)
    select('tree', 1, tree, list, 'home')
    tree.redraw(true)
    list.redraw(true)

    @@top = TkToplevel.new
    @@top.withdraw
    @@top.protocol('WM_DELETE_WINDOW'){
      # don't kill me
    }
    @@top.resizable(false, false)
    @@top.title('Drag rectangle to scroll directory tree')
    @@top.transient(Tk.root)
    Tk::BWidget::ScrollView.new(@@top, :window=>tree, :fill=>'white',
                                :width=>300, :height=>300, :relief=>:sunken,
                                :bd=>1).pack(:fill=>:both, :expand=>true)
  end

  def self.getdir(tree, node, path)
    lentries = Dir.glob(File.join(path, '*')).sort
    lfiles = []
    lentries.each{|f|
      basename = File.basename(f)
      if File.directory?(f)
        Tk::BWidget::Tree::Node.new(tree, node,
                                    :index=>'end', :text=>basename,
                                    :image=>Tk::BWidget::Bitmap.new('folder'),
                                    :drawcross=>:allways, :data=>f)
        @@count += 1
      else
        lfiles << basename
      end
    }
    tree.itemconfigure(node, :drawcross=>:auto, :data=>lfiles)
  end

  def self.moddir(idx, tree, node)
    if (idx != 0 && tree.itemcget(node, :drawcross).to_s == 'allways')
      getdir(tree, node, tree.itemcget(node, :data))
      if tree.nodes(node).empty?
        tree.itemconfigure(node, :image=>Tk::BWidget::Bitmap.new('folder'))
      else
        tree.itemconfigure(node, :image=>Tk::BWidget::Bitmap.new('openfold'))
      end
    else
      img = %w(folder openfold)[idx] || 'openfold'
      tree.itemconfigure(node, :image=>Tk::BWidget::Bitmap.new(img))
    end
  end

  def self.select(where, num, tree, list, node)
    @@dblclick = true
    if num == 1
      if (where == 'tree' &&
          tree.selection_get.find{|x|
            TkUtil._get_eval_string(x) == TkUtil._get_eval_string(node)
          })
        @@dblclick = false
        Tk.after(500, proc{edit('tree', tree, list, node)})
        return
      end
      if (where == 'list' &&
          list.selection_get.find{|x|
            TkUtil._get_eval_string(x) == TkUtil._get_eval_string(node)
          })
        @@dblclick = false
        Tk.after(500, proc{edit('list', tree, list, node)})
        return
      end
      if where == 'tree'
        select_node(tree, list, node)
      else
        list.selection_set(node)
      end
    elsif (where == 'list' && tree.exist?(node))
      parent = tree.parent(node)
      while TkUtil._get_eval_string(parent) != 'root'
        tree.itemconfigure(parent, :open=>true)
        parent = tree.parent(parent)
      end
      select_node(tree, list, node)
    end
  end

  def self.select_node(tree, list, node)
    tree.selection_set(node)
    Tk.update
    list.delete(*(list.items(0, 'end')))

    dir = tree.itemcget(node, :data)
    if tree.itemcget(node, :drawcross).to_s == 'allways'
      getdir(tree, node, dir)
      dir = tree.itemcget(node, :data)
    end

    tree.nodes(node).each{|subnode|
      list.insert('end', subnode,
                  :text=>tree.itemcget(subnode, :text),
                  :image=>Tk::BWidget::Bitmap.new('folder'))
    }

    TkComm.simplelist(dir).each{|f|
      Tk::BWidget::ListBox::Item.new(list, 'end', :text=>f,
                                     :image=>Tk::BWidget::Bitmap.new('file'))
    }
  end

  def self.edit(where, tree, list, node)
    return if @@dblclick

    if (where == 'tree' &&
        tree.selection_get.find{|x|
          TkUtil._get_eval_string(x) == TkUtil._get_eval_string(node)
        })
      res = tree.edit(node, tree.itemcget(node, :text))
      if res != ''
        tree.itemconfigure(node, :text=>res)
        if list.exist?(node)
          list.itemconfigure(node, :text=>res)
        end
        tree.selection_set(node)
      end
      return
    end

    if (where == 'list')
      res = list.edit(node, list.igemcget(node, :text))
      if res != ''
        list.itemconfigure(node, :text=>res)
        if tree.exist?(node)
          tree.itemconfigure(node, :text=>res)
        else
          cursel = tree.selection_get[0]
          index  = list.index(node) - tree.nodes(cursel).size
          data   = TkComm.simplelist(tree.itemcget(cursel, :data))
          data[index] = res
          tree.itemconfigure(cursel, :date=>data)
        end
        list.selection_set(node)
      end
    end
  end

  def self.expand(tree, but)
    unless (cur = tree.selection_get).empty?
      if TkComm.bool(but)
        tree.opentree(cur)
      else
        tree.closetree(cur)
      end
    end
  end
end
