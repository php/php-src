#
#  tkextlib/blt/tree.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/blt.rb'

module Tk::BLT
  class Tree < TkObject
    TkCommandNames = ['::blt::tree'.freeze].freeze

    ###################################

    class Node < TkObject
      TreeNodeID_TBL = TkCore::INTERP.create_table

      TkCore::INTERP.init_ip_env{
        TreeNodeID_TBL.mutex.synchronize{ TreeNodeID_TBL.clear }
      }

      def self.id2obj(tree, id)
        tpath = tree.path
        TreeNodeID_TBL.mutex.synchronize{
          if TreeNodeID_TBL[tpath]
            if TreeNodeID_TBL[tpath][id]
              TreeNodeID_TBL[tpath][id]
            else
              begin
                # self.new(tree, nil, 'node'=>Integer(id))
                id = Integer(id)
                if bool(tk_call(@tpath, 'exists', id))
                  (obj = self.allocate).instance_eval{
                    @parent = @tree = tree
                    @tpath = tpath
                    @path = @id = id
                    TreeNodeID_TBL[@tpath] = {} unless TreeNodeID_TBL[@tpath]
                    TreeNodeID_TBL[@tpath][@id] = self
                  }
                  obj
                else
                  id
                end
              rescue
                id
              end
            end
          else
            id
          end
        }
      end

      def self.new(tree, parent, keys={})
        keys = _symbolkey2str(keys)
        tpath = tree.path

        TreeNodeID_TBL.mutex.synchronize{
          TreeNodeID_TBL[tpath] ||= {}
          if (id = keys['node']) && (obj = TreeNodeID_TBL[tpath][id])
            keys.delete('node')
            tk_call(tree.path, 'move', id, parent, keys) if parent
            return obj
          end

          (obj = self.allocate).instance_eval{
            initialize(tree, parent, keys)
            TreeNodeID_TBL[tpath][@id] = self
          }
          obj
        }
      end

      def initialize(tree, parent, keys={})
        @parent = @tree = tree
        @tpath = @parent.path

        if (id = keys['node']) && bool(tk_call(@tpath, 'exists', id))
          @path = @id = id
          keys.delete('node')
          tk_call(@tpath, 'move', @id, parent, keys) if parent
        else
          parent = tk_call(@tpath, 'root') unless parent
          @path = @id = tk_call(@tpath, 'insert', parent, keys)
        end
      end

      def id
        @id
      end

      def apply(keys={})
        @tree.apply(@id, keys)
        self
      end

      def children()
        @tree.children(@id)
      end

      def copy(parent, keys={})
        @tree.copy(@id, parent, keys)
      end
      def copy_to(dest_tree, parent, keys={})
        @tree.copy_to(@id, dest_tree, parent, keys)
      end

      def degree()
        @tree.degree(@id)
      end

      def delete()
        @tree.delete(@id)
        self
      end

      def depth()
        @tree.depth(@id)
      end

      def dump()
        @tree.dump(@id)
      end

      def dump_to_file(file)
        @tree.dump_to_file(@id, file)
        self
      end

      def exist?(keys={})
        @tree.exist?(@id, keys)
      end

      def find(keys={})
        @tree.find(@id, keys)
      end

      def find_child(label)
        @tree.find_child(@id, label)
      end

      def first_child()
        @tree.first_child(@id)
      end

      def get()
        @tree.get(@id)
      end
      def get_value(key, default_val=None)
        @tree.get_value(@id, key, default_val)
      end

      def index()
        @tree.index(@id)
      end

      def leaf?()
        @tree.leaf?(@id)
      end
      def link?()
        @tree.link?(@id)
      end
      def root?()
        @tree.root?(@id)
      end

      def keys()
        @tree.keys(@id)
      end

      def label(text = nil)
        @tree.label(@id, nil)
      end
      def label=(text)
        @tree.label(@id, text)
      end

      def last_child()
        @tree.last_child(@id)
      end

      def move(dest, keys={})
        @tree.keys(@id, dest, keys)
        self
      end

      def next()
        @tree.next(@id)
      end

      def next_sibling()
        @tree.next_sibling(@id)
      end

      def parent()
        @tree.parent(@id)
      end

      def fullpath()
        @tree.fullpath(@id)
      end

      def position()
        @tree.position(@id)
      end

      def previous()
        @tree.previous(@id)
      end

      def prev_sibling()
        @tree.prev_sibling(@id)
      end

      def restore(str, keys={})
        @tree.restore(@id, str, keys)
        self
      end
      def restore_overwrite(str, keys={})
        @tree.restore_overwrite(@id, str, keys)
        self
      end

      def restore_from_file(file, keys={})
        @tree.restore_from_file(@id, file, keys)
        self
      end
      def restore_overwrite_from_file(file, keys={})
        @tree.restore_overwrite_from_file(@id, file, keys)
        self
      end

      def root()
        @tree.root(@id)
        self
      end

      def set(data)
        @tree.set(@id, data)
        self
      end

      def size()
        @tree.size(@id)
      end

      def sort(keys={})
        @tree.sort(@id, keys)
        self
      end

      def type(key)
        @tree.type(@id, key)
      end

      def unset(*keys)
        @tree.unset(@id, *keys)
        self
      end

      def values(key=None)
        @tree.values(@id, key)
      end
    end

    ###################################

    class Tag < TkObject
      TreeTagID_TBL = TkCore::INTERP.create_table

      TkCore::INTERP.init_ip_env{
        TreeTagID_TBL.mutex.synchronize{ TreeTagID_TBL.clear }
      }

      (TreeTag_ID = ['blt_tree_tag'.freeze, TkUtil.untrust('00000')]).instance_eval{
        @mutex = Mutex.new
        def mutex; @mutex; end
        freeze
      }

      def self.id2obj(tree, id)
        tpath = tree.path
        TreeTagID_TBL.mutex.synchronize{
          if TreeTagID_TBL[tpath]
            if TreeTagID_TBL[tpath][id]
              TreeTagID_TBL[tpath][id]
            else
              begin
                # self.new(tree, id)
                (obj = self.allocate).instance_eval{
                  @parent = @tree = tree
                  @tpath = @parent.path
                  @path = @id = id.dup.freeze if id
                  TreeTagID_TBL[@tpath] = {} unless TreeTagID_TBL[@tpath]
                  TreeTagID_TBL[@tpath][@id] = self
                }
                obj
              rescue
                id
              end
            end
          else
            id
          end
        }
      end

      def initialize(tree, tag_str = nil)
        @parent = @tree = tree
        @tpath = @parent.path

        if tag_str
          @path = @id = tag_str.dup.freeze
        else
          TreeTag_ID.mutex.synchronize{
            @path = @id = TreeTag_ID.join(TkCore::INTERP._ip_id_)
            TreeTag_ID[1].succ!
          }
        end
        TreeTagID_TBL.mutex.synchronize{
          TreeTagID_TBL[@tpath] = {} unless TreeTagID_TBL[@tpath]
          TreeTagID_TBL[@tpath][@id] = self
        }
      end

      def id
        @id
      end

      def add(*nodes)
        tk_call(@tpath, 'tag', 'add', @id, *nodes)
        self
      end

      def delete(*nodes)
        tk_call(@tpath, 'tag', 'delete', @id, *nodes)
        self
      end

      def forget()
        tk_call(@tpath, 'tag', 'forget', @id)
        TreeTagID_TBL.mutex.synchronize{
          TreeTagID_TBL[@tpath].delete(@id)
        }
        self
      end

      def nodes()
        simplelist(tk_call(@tpath, 'tag', 'nodes', @id)).collect{|node|
          Tk::BLT::Tree::Node.id2obj(@path, node)
        }
      end

      def set(node)
        tk_call(@tpath, 'tag', 'set', node, @id)
        self
      end

      def unset(node)
        tk_call(@tpath, 'tag', 'unset', node, @id)
        self
      end
    end

    ###################################

    class Notify < TkObject
      NotifyID_TBL = TkCore::INTERP.create_table

      TkCore::INTERP.init_ip_env{
        NotifyID_TBL.mutex.synchronize{ NotifyID_TBL.clear }
      }

      def self.id2obj(tree, id)
        tpath = tree.path
        NotifyID_TBL.mutex.synchronize{
          if NotifyID_TBL[tpath]
            if NotifyID_TBL[tpath][id]
              NotifyID_TBL[tpath][id]
            else
              (obj = self.allocate).instance_eval{
                @parent = @tree = tree
                @tpath = @parent.path
                @path = @id = id
                NotifyID_TBL[@tpath] ||= {}
                NotifyID_TBL[@tpath][@id] = self
              }
              obj
            end
          else
            return id
          end
        }
      end

      def self.new(tree, *args, &b)
        NotifyID_TBL.mutex.synchronize{
          if tree.kind_of?(Array)
            # not create
            tpath = tree[0].path
            NotifyID_TBL[tpath] ||= {}
            unless (obj = NotifyID_TBL[tpath][tree[1]])
              (NotifyID_TBL[tpath][tree[1]] =
                 obj = self.allocate).instance_eval{
                @parent = @tree = tree[0]
                @tpath = @parent.path
                @path = @id = tree[1]
              }
            end
            return obj
          end

          (obj = self.allocate).instance_eval{
            initialize(tree, *args, &b)
            NotifyID_TBL[@tpath] ||= {}
            NotifyID_TBL[@tpath][@id] = self
          }
          return obj
        }
      end

      def initialize(tree, *args, &b)
        @parent = @tree = tree
        @tpath = @parent.path

        # if args[0].kind_of?(Proc) || args[0].kind_of?(Method)
        if TkComm._callback_entry?(args[0])
          cmd = args.shift
        # elsif args[-1].kind_of?(Proc) || args[-1].kind_of?(Method)
        elsif TkComm._callback_entry?(args[-1])
          cmd = args.pop
        elsif b
          cmd = Proc.new(&b)
        else
          fail ArgumentError, "lack of 'command' argument"
        end

        args = args.collect{|arg| '-' << arg.to_s}

        args << proc{|id, type|
          cmd.call(Tk::BLT::Tree::Node.id2obj(@tree, id),
                   ((type[0] == ?-)? type[1..-1]: type))
        }

        @path = @id = tk_call(@tpath, 'notify', 'create', *args)
      end

      def id
        @id
      end

      def delete()
        tk_call(@tpath, 'notify', 'delete', @id)
        NotifyID_TBL.mutex.synchronize{
          NotifyID_TBL[@tpath].delete(@id)
        }
        self
      end

      def info()
        lst = simplelist(tk_call(@tpath, 'notify', 'info', id))
        lst[0] = Tk::BLT::Tree::Notify.id2obj(@tree, lst[0])
        lst[1] = simplelist(lst[1]).collect{|flag| flag[1..-1]}
        lst[2] = tk_tcl2ruby(lst[2])
        lst
      end
    end

    ###################################

    class Trace < TkObject
      TraceID_TBL = TkCore::INTERP.create_table

      TkCore::INTERP.init_ip_env{
        TraceID_TBL.mutex.synchronize{ TraceID_TBL.clear }
      }

      def self.id2obj(tree, id)
        tpath = tree.path
        TraceID_TBL.mutex.synchronize{
          if TraceID_TBL[tpath]
            if TraceID_TBL[tpath][id]
              TraceID_TBL[tpath][id]
            else
              begin
                # self.new([tree, id])
                (obj = self.allocate).instance_eval{
                  @parent = @tree = tree
                  @tpath = @parent.path
                  @path = @id = node  # == traceID
                  TraceID_TBL[@tpath] ||= {}
                  TraceID_TBL[@tpath][@id] = self
                }
                obj
              rescue
                id
              end
            end
          else
            id
          end
        }
      end

      def self.new(tree, *args, &b)
        TraceID_TBL.mutex.synchronize{
          if tree.kind_of?(Array)
            # not create
            tpath = tree[0].path
            TraceID_TBL[tpath] ||= {}
            unless (obj = TraceID_TBL[tpath][tree[1]])
              (TraceID_TBL[tpath][tree[1]] =
                 obj = self.allocate).instance_eval{
                @parent = @tree = tree
                @tpath = @parent.path
                @path = @id = tree[1]  # == traceID
              }
            end
            return obj
          end

          # super(true, tree, *args, &b)
          (obj = self.allocate).instance_eval{
            initialize(tree, *args, &b)
            TraceID_TBL[@tpath] ||= {}
            TraceID_TBL[@tpath][@id] = self
          }
          return obj
        }
      end

      def initialize(tree, node, key, opts, cmd=nil, &b)
        @parent = @tree = tree
        @tpath = @parent.path

        if !cmd
          if b
            cmd = Proc.new(&b)
          else
            fail ArgumentError, "lack of 'command' argument"
          end
        end

        @path = @id = tk_call(@tpath, 'trace', 'create', node, key, opts,
                              proc{|t, id, k, ops|
                                tobj = Tk::BLT::Tree.id2obj(t)
                                if tobj.kind_of?(Tk::BLT::Tree)
                                  nobj = Tk::BLT::Tree::Node.id2obj(tobj, id)
                                else
                                  nobj = id
                                end
                                cmd.call(tobj, nobj, k, ops)
                              })
      end

      def id
        @id
      end

      def delete()
        tk_call(@tpath, 'trace', 'delete', @id)
        TraceID_TBL.mutex.synchronize{
          TraceID_TBL[tpath].delete(@id)
        }
        self
      end

      def info()
        lst = simplelist(tk_call(@tpath, 'trace', 'info', id))
        lst[0] = Tk::BLT::Tree::Trace.id2obj(@tree, lst[0])
        lst[2] = simplelist(lst[2])
        lst[3] = tk_tcl2ruby(lst[3])
        lst
      end
    end

    ###################################

    TreeID_TBL = TkCore::INTERP.create_table

    (Tree_ID = ['blt_tree'.freeze, TkUtil.untrust('00000')]).instance_eval{
      @mutex = Mutex.new
      def mutex; @mutex; end
      freeze
    }

    def __keyonly_optkeys
      {
        # apply / find  command
        'invert'=>nil, 'leafonly'=>nil, 'nocase'=>nil,

        # apply / find / sort command
        'path'=>nil,

        # copy / restore / restorefile command
        'overwrite'=>nil,

        # copy command
        'recurse'=>nil, 'tags'=>nil,

        # sort command
        'ascii'=>nil, 'decreasing'=>nil, 'disctionary'=>nil,
        'integer'=>nil, 'real'=>nil, 'recurse'=>nil, 'reorder'=>nil,
      }
    end

    def self.id2obj(id)
      TreeID_TBL.mutex.synchronize{
        TreeID_TBL[id]? TreeID_TBL[id]: id
      }
    end

    def self.names(pat = None)
      simplelist(tk_call('::blt::tree', 'names', pat)).collect{|name|
        id2obj(name)
      }
    end

    def self.destroy(*names)
      tk_call('::blt::tree', 'destroy',
              *(names.collect{|n| (n.kind_of?(Tk::BLT::Tree))? n.id: n }) )
    end

    def self.new(name = nil)
      TreeID_TBL.mutex.synchronize{
        if name && TreeID_TBL[name]
          TreeID_TBL[name]
        else
          (obj = self.allocate).instance_eval{
            initialize(name)
            TreeID_TBL[@id] = self
          }
          obj
        end
      }
    end

    def initialzie(name = nil)
      if name
        @path = @id = name
      else
        Tree_ID.mutex.synchronize{
          @path = @id = Tree_ID.join(TkCore::INTERP._ip_id_)
          Tree_ID[1].succ!
        }
      end

      tk_call('::blt::tree', 'create', @id)
    end

    def __destroy_hook__
      Tk::BLT::Tree::Node::TreeNodeID_TBL.mutex.synchronize{
        Tk::BLT::Tree::Node::TreeNodeID_TBL.delete(@path)
      }
      Tk::BLT::Tree::Tag::TreeTagID_TBL.mutex.synchronize{
        Tk::BLT::Tree::Tag::TreeTagID_TBL.delete(@path)
      }
      Tk::BLT::Tree::Notify::NotifyID_TBL.mutex.synchronize{
        Tk::BLT::Tree::Notify::NotifyID_TBL.delete(@path)
      }
      Tk::BLT::Tree::Trace::TraceID_TBL.mutex.synchronize{
        Tk::BLT::Tree::Trace::TraceID_TBL.delete(@path)
      }
    end

    def tagid(tag)
      if tag.kind_of?(Tk::BLT::Tree::Node) ||
          tag.kind_of?(Tk::BLT::Tree::Tag) ||
          tag.kind_of?(Tk::BLT::Tree::Notify) ||
          tag.kind_of?(Tk::BLT::Tree::Trace)
        tag.id
      else
        tag  # maybe an Array of configure paramters
      end
    end

    def destroy()
      tk_call('::blt::tree', 'destroy', @id)
      self
    end

    def ancestor(node1, node2)
      Tk::BLT::Tree::Node.id2obj(self, tk_call('::blt::tree', 'ancestor',
                                               tagid(node1), tagid(node2)))
    end

    def apply(node, keys={})
      tk_call('::blt::tree', 'apply', tagid(node), __conv_keyonly_opts(keys))
      self
    end

    def attach(tree_obj)
      tk_call('::blt::tree', 'attach', tree_obj)
      self
    end

    def children(node)
      simplelist(tk_call('::blt::tree', 'children', tagid(node))).collect{|n|
        Tk::BLT::Tree::Node.id2obj(self, n)
      }
    end

    def copy(src, parent, keys={})
      id = tk_call('::blt::tree', 'copy', tagid(src), tagid(parent),
                   __conv_keyonly_opts(keys))
      Tk::BLT::Tree::Node.new(self, nil, 'node'=>id)
    end
    def copy_to(src, dest_tree, parent, keys={})
      return copy(src, parent, keys={}) unless dest_tree

      id = tk_call('::blt::tree', 'copy', tagid(src), dest_tree,
                   tagid(parent), __conv_keyonly_opts(keys))
      Tk::BLT::Tree::Node.new(dest_tree, nil, 'node'=>id)
    end

    def degree(node)
      number(tk_call('::blt::tree', 'degree', tagid(node)))
    end

    def delete(*nodes)
      tk_call('::blt::tree', 'delete', *(nodes.collect{|node| tagid(node)}))
      Tk::BLT::Tree::Node::TreeNodeID_TBL.mutex.synchronize{
        nodes.each{|node|
          if node.kind_of?(Tk::BLT::Tree::Node)
            Tk::BLT::Tree::Node::TreeNodeID_TBL[@path].delete(node.id)
          else
            Tk::BLT::Tree::Node::TreeNodeID_TBL[@path].delete(node.to_s)
          end
        }
      }
      self
    end

    def depth(node)
      number(tk_call('::blt::tree', 'depth', tagid(node)))
    end

    def dump(node)
      simplelist(tk_call('::blt::tree', 'dump', tagid(node))).collect{|n|
        simplelist(n)
      }
    end

    def dump_to_file(node, file)
      tk_call('::blt::tree', 'dumpfile', tagid(node), file)
      self
    end

    def exist?(node, key=None)
      bool(tk_call('::blt::tree', 'exists', tagid(node), key))
    end

    def find(node, keys={})
      simplelist(tk_call('::blt::tree', 'find', tagid(node),
                         __conv_keyonly_opts(keys))).collect{|n|
        Tk::BLT::Tree::Node.id2obj(self, n)
      }
    end

    def find_child(node, label)
      ret = tk_call('::blt::tree', 'findchild', tagid(node), label)
      (ret == '-1')? nil: Tk::BLT::Tree::Node.id2obj(self, ret)
    end

    def first_child(node)
      ret = tk_call('::blt::tree', 'firstchild', tagid(node))
      (ret == '-1')? nil: Tk::BLT::Tree::Node.id2obj(self, ret)
    end

    def get(node)
      Hash[*simplelist(tk_call('::blt::tree', 'get', tagid(node)))]
    end
    def get_value(node, key, default_val=None)
      tk_call('::blt::tree', 'get', tagid(node), key, default_val)
    end

    def index(node)
      Tk::BLT::Tree::Node.id2obj(self,
                                 tk_call('::blt::tree', 'index', tagid(node)))
    end

    def insert(parent, keys={})
      id = tk_call('::blt::tree', 'insert', tagid(parent), keys)
      Tk::BLT::Tree::Node.new(self, nil, 'node'=>id)
    end

    def ancestor?(node1, node2)
      bool(tk_call('::blt::tree', 'is', 'ancestor',
                   tagid(node1), tagid(node2)))
    end
    def before?(node1, node2)
      bool(tk_call('::blt::tree', 'is', 'before',
                   tagid(node1), tagid(node2)))
    end
    def leaf?(node)
      bool(tk_call('::blt::tree', 'is', 'leaf', tagid(node)))
    end
    def link?(node)
      bool(tk_call('::blt::tree', 'is', 'link', tagid(node)))
    end
    def root?(node)
      bool(tk_call('::blt::tree', 'is', 'root', tagid(node)))
    end

    def keys(node, *nodes)
      if nodes.empty?
        simplelist(tk_call('blt::tree', 'keys', tagid(node)))
      else
        simplelist(tk_call('blt::tree', 'keys', tagid(node),
                           *(nodes.collect{|n| tagid(n)}))).collect{|lst|
          simplelist(lst)
        }
      end
    end

    def label(node, text=nil)
      if text
        tk_call('::blt::tree', 'label', tagid(node), text)
        text
      else
        tk_call('::blt::tree', 'label', tagid(node))
      end
    end

    def last_child(node)
      ret = tk_call('::blt::tree', 'lastchild', tagid(node))
      (ret == '-1')? nil: Tk::BLT::Tree::Node.id2obj(self, ret)
    end

    def link(parent, node, keys={})
      ret = tk_call('::blt::tree', 'link', tagid(parent), tagid(node),
                    __conv_keyonly_opts(keys))
      (ret == '-1')? nil: Tk::BLT::Tree::Node.id2obj(self, ret)
    end

    def move(node, dest, keys={})
      tk_call('::blt::tree', 'move', tagid(node), tagid(dest), keys)
      self
    end

    def next(node)
      ret = tk_call('::blt::tree', 'next', tagid(node))
      (ret == '-1')? nil: Tk::BLT::Tree::Node.id2obj(self, ret)
    end

    def next_sibling(node)
      ret = tk_call('::blt::tree', 'nextsibling', tagid(node))
      (ret == '-1')? nil: Tk::BLT::Tree::Node.id2obj(self, ret)
    end

    def notify_create(*args, &b)
      Tk::BLT::Tree::Notify.new(self, *args, &b)
    end

    def notify_delete(id)
      if id.kind_of?(Tk::BLT::Tree::Notify)
        id.delete
      else
        tk_call(@path, 'notify', 'delete', id)
        Tk::BLT::Tree::Notify::NotifyID_TBL.mutex.synchronize{
          Tk::BLT::Tree::Notify::NotifyID_TBL[@path].delete(id.to_s)
        }
      end
      self
    end

    def notify_info(id)
      lst = simplelist(tk_call(@path, 'notify', 'info', tagid(id)))
      lst[0] = Tk::BLT::Tree::Notify.id2obj(self, lst[0])
      lst[1] = simplelist(lst[1]).collect{|flag| flag[1..-1]}
      lst[2] = tk_tcl2ruby(lst[2])
      lst
    end

    def notify_names()
      tk_call(@path, 'notify', 'names').collect{|id|
        Tk::BLT::Tree::Notify.id2obj(self, id)
      }
    end

    def parent(node)
      ret = tk_call('::blt::tree', 'parent', tagid(node))
      (ret == '-1')? nil: Tk::BLT::Tree::Node.id2obj(self, ret)
    end

    def fullpath(node)
      tk_call('::blt::tree', 'path', tagid(node))
    end

    def position(node)
      number(tk_call('::blt::tree', 'position', tagid(node)))
    end

    def previous(node)
      ret = tk_call('::blt::tree', 'previous', tagid(node))
      (ret == '-1')? nil: Tk::BLT::Tree::Node.id2obj(self, ret)
    end

    def prev_sibling(node)
      ret = tk_call('::blt::tree', 'prevsibling', tagid(node))
      (ret == '-1')? nil: Tk::BLT::Tree::Node.id2obj(self, ret)
    end

    def restore(node, str, keys={})
      tk_call('::blt::tree', 'restore', tagid(node), str,
              __conv_keyonly_opts(keys))
      self
    end
    def restore_overwrite(node, str, keys={})
      keys = __conv_keyonly_opts(keys)
      keys.delete('overwrite')
      keys.delete(:overwrite)
      tk_call('::blt::tree', 'restore', tagid(node), str, '-overwrite', keys)
      self
    end

    def restore_from_file(node, file, keys={})
      tk_call('::blt::tree', 'restorefile', tagid(node), file,
              __conv_keyonly_opts(keys))
      self
    end
    def restore_overwrite_from_file(node, file, keys={})
      keys = __conv_keyonly_opts(keys)
      keys.delete('overwrite')
      keys.delete(:overwrite)
      tk_call('::blt::tree', 'restorefile', tagid(node), file,
              '-overwrite', keys)
      self
    end

    def root(node=None)
      Tk::BLT::Tree::Node.id2obj(self, tk_call('::blt::tree', 'root',
                                               tagid(node)))
    end

    def set(node, data)
      unless data.kind_of?(Hash)
        fail ArgumentError, 'Hash is expected for data'
      end
      args = []
      data.each{|k, v|  args << k << v}
      tk_call('::blt::tree', 'set', tagid(node), *args)
      self
    end

    def size(node)
      number(tk_call('::blt::tree', 'size', tagid(node)))
    end

    def sort(node, keys={})
      tk_call('::blt::tree', 'sort', tagid(node), __conv_keyonly_opts(keys))
      self
    end

    def tag_add(tag, *nodes)
      tk_call(@path, 'tag', 'add', tagid(tag), *(nodes.collect{|n| tagid(n)}))
      self
    end

    def tag_delete(tag, *nodes)
      tk_call(@path, 'tag', 'delete', tagid(tag),
              *(nodes.collect{|n| tagid(n)}))
      self
    end

    def tag_forget(tag)
      tag = tag.id if tag.kind_of?(Tk::BLT::Tree::Tag)
      tk_call(@path, 'tag', 'forget', tag)
      TreeTagID_TBL.mutex.synchronize{
        TreeTagID_TBL[@path].delete(tag)
      }
      self
    end

    def tag_get(node, *patterns)
      simplelist(tk_call(@tpath, 'tag', 'get', tagid(node),
                         *(patterns.collect{|pat| tagid(pat)}))).collect{|str|
        Tk::BLT::Tree::Tag.id2obj(self, str)
      }
    end

    def tag_names(node = None)
      simplelist(tk_call(@tpath, 'tag', 'names', tagid(node))).collect{|str|
        Tk::BLT::Tree::Tag.id2obj(self, str)
      }
    end

    def tag_nodes(tag)
      simplelist(tk_call(@tpath, 'tag', 'nodes', tagid(tag))).collect{|node|
        Tk::BLT::Tree::Node.id2obj(self, node)
      }
    end

    def tag_set(node, *tags)
      tk_call(@path, 'tag', 'set', tagid(node), *(tags.collect{|t| tagid(t)}))
      self
    end

    def tag_unset(node, *tags)
      tk_call(@path, 'tag', 'unset', tagid(node),
              *(tags.collect{|t| tagid(t)}))
      self
    end

    def trace_create(*args, &b)
      Tk::BLT::Tree::Trace.new(self, *args, &b)
    end

=begin
    def trace_delete(*args)
      args.each{|id|
        if id.kind_of?(Tk::BLT::Tree::Trace)
          id.delete
        else
          tk_call(@path, 'trace', 'delete', id)
          Tk::BLT::Tree::Trace::TraceID_TBL[@path].delete(id.to_s)
        end
        self
      }
    end
=end
    def trace_delete(*args)
      args = args.collect{|id| tagid(id)}
      tk_call(@path, 'trace', 'delete', *args)
      Tk::BLT::Tree::Trace::TraceID_TBL.mutex.synchronize{
        args.each{|id| Tk::BLT::Tree::Trace::TraceID_TBL[@path].delete(id.to_s)}
      }
      self
    end

    def trace_info(id)
      lst = simplelist(tk_call(@path, 'trace', 'info', tagid(id)))
      lst[0] = Tk::BLT::Tree::Trace.id2obj(self, lst[0])
      lst[2] = simplelist(lst[2])
      lst[3] = tk_tcl2ruby(lst[3])
      lst
    end

    def trace_names()
      tk_call(@path, 'trace', 'names').collect{|id|
        Tk::BLT::Tree::Trace.id2obj(self, id)
      }
    end

    def type(node, key)
      tk_call('::blt::tree', 'type', tagid(node), key)
    end

    def unset(node, *keys)
      tk_call('::blt::tree', 'unset', tagid(node), *keys)
      self
    end

    def values(node, key=None)
      simplelist(tk_call('::blt::tree', 'values', tagid(node), key))
    end
  end
end
