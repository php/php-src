#
#  tkextlib/blt/vector.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/blt.rb'

module Tk::BLT
  class Vector < TkVariable
    TkCommandNames = ['::blt::vector'.freeze].freeze

    def self.create(*args)
      tk_call('::blt::vector', 'create', *args)
    end

    def self.destroy(*args)
      tk_call('::blt::vector', 'destroy', *args)
    end

    def self.expr(expression)
      tk_call('::blt::vector', 'expr', expression)
    end

    def self.names(pat=None)
      list = simplelist(tk_call('::blt::vector', 'names', pat))
      TkVar_ID_TBL.mutex.synchronize{
        list.collect{|name|
          if TkVar_ID_TBL[name]
            TkVar_ID_TBL[name]
          elsif name[0..1] == '::' && TkVar_ID_TBL[name[2..-1]]
            TkVar_ID_TBL[name[2..-1]]
          else
            name
          end
        }
      }
    end

    ####################################

    def initialize(size=nil, keys={})
      if size.kind_of?(Hash)
        keys = size
        size = nil
      end
      if size.kind_of?(Array)
        # [first, last]
        size = size.join(':')
      end
      if size
        @id = TkCore::INTERP._invoke('::blt::vector', 'create',
                                     "#auto(#{size})", *hash_kv(keys))
      else
        @id = TkCore::INTERP._invoke('::blt::vector', 'create',
                                     "#auto", *hash_kv(keys))
      end

      TkVar_ID_TBL.mutex.synchronize{
        TkVar_ID_TBL[@id] = self
      }

      @def_default = false
      @default_val = nil

      @trace_var  = nil
      @trace_elem = nil
      @trace_opts = nil

      # teach Tk-ip that @id is global var
      TkCore::INTERP._invoke_without_enc('global', @id)
    end

    def destroy
      tk_call('::blt::vector', 'destroy', @id)
    end

    def inspect
      '#<Tk::BLT::Vector: ' + @id + '>'
    end

    def to_s
      @id
    end

    def *(item)
      list(tk_call(@id, '*', item))
    end

    def +(item)
      list(tk_call(@id, '+', item))
    end

    def -(item)
      list(tk_call(@id, '-', item))
    end

    def /(item)
      list(tk_call(@id, '/', item))
    end

    def append(*vectors)
      tk_call(@id, 'append', *vectors)
    end

    def binread(channel, len=None, keys={})
      if len.kind_of?(Hash)
        keys = len
        len = None
      end
      keys = _symbolkey2str(keys)
      keys['swap'] = None if keys.delete('swap')
      tk_call(@id, 'binread', channel, len, keys)
    end

    def clear()
      tk_call(@id, 'clear')
      self
    end

    def delete(*indices)
      tk_call(@id, 'delete', *indices)
      self
    end

    def dup_vector(vec)
      tk_call(@id, 'dup', vec)
      self
    end

    def expr(expression)
      tk_call(@id, 'expr', expression)
      self
    end

    def index(idx, val=None)
      number(tk_call(@id, 'index', idx, val))
    end

    def [](idx)
      index(idx)
    end

    def []=(idx, val)
      index(idx, val)
    end

    def length()
      number(tk_call(@id, 'length'))
    end

    def length=(size)
      number(tk_call(@id, 'length', size))
    end

    def merge(*vectors)
      tk_call(@id, 'merge', *vectors)
      self
    end

    def normalize(vec=None)
      tk_call(@id, 'normalize', vec)
      self
    end

    def notify(keyword)
      tk_call(@id, 'notify', keyword)
      self
    end

    def offset()
      number(tk_call(@id, 'offset'))
    end

    def offset=(val)
      number(tk_call(@id, 'offset', val))
    end

    def random()
      tk_call(@id, 'random')
    end

    def populate(vector, density=None)
      tk_call(@id, 'populate', vector, density)
      self
    end

    def range(first, last=None)
      list(tk_call(@id, 'range', first, last))
    end

    def search(val1, val2=None)
      list(tk_call(@id, 'search', val1, val2))
    end

    def set(item)
      tk_call(@id, 'set', item)
      self
    end

    def seq(start, finish=None, step=None)
      tk_call(@id, 'seq', start, finish, step)
      self
    end

    def sort(*vectors)
      tk_call(@id, 'sort', *vectors)
      self
    end

    def sort_reverse(*vectors)
      tk_call(@id, 'sort', '-reverse', *vectors)
      self
    end

    def split(*vectors)
      tk_call(@id, 'split', *vectors)
      self
    end

    def variable(var)
      tk_call(@id, 'variable', var)
      self
    end
  end

  class VectorAccess < Vector
    def self.new(name)
      TkVar_ID_TBL.mutex.synchronize{
        if TkVar_ID_TBL[name]
          TkVar_ID_TBL[name]
        else
          (obj = self.allocate).instance_eval{
            initialize(name)
            TkVar_ID_TBL[@id] = self
          }
          obj
        end
      }
    end

    def initialize(vec_name)
      @id = vec_name

      @def_default = false
      @default_val = nil

      @trace_var  = nil
      @trace_elem = nil
      @trace_opts = nil

      # teach Tk-ip that @id is global var
      TkCore::INTERP._invoke_without_enc('global', @id)
    end
  end
end
