#
# tk/bind.rb : control event binding
#
require 'tk'

class TkBindTag
  include TkBindCore

  #BTagID_TBL = {}
  BTagID_TBL = TkCore::INTERP.create_table

  (Tk_BINDTAG_ID = ["btag".freeze, TkUtil.untrust("00000")]).instance_eval{
    @mutex = Mutex.new
    def mutex; @mutex; end
    freeze
  }

  TkCore::INTERP.init_ip_env{
    BTagID_TBL.mutex.synchronize{ BTagID_TBL.clear }
  }

  def TkBindTag.id2obj(id)
    BTagID_TBL.mutex.synchronize{
      (BTagID_TBL[id])? BTagID_TBL[id]: id
    }
  end

=begin
  def TkBindTag.new_by_name(name, *args, &b)
    BTagID_TBL.mutex.synchronize{
      return BTagID_TBL[name] if BTagID_TBL[name]
    }

    self.new.instance_eval{
      BTagID_TBL.mutex.synchronize{
        BTagID_TBL.delete @id
        @id = name
        BTagID_TBL[@id] = self
      }
      bind(*args, &b) if args != []
      self
    }
  end
=end
  def TkBindTag.new_by_name(name, *args, &b)
    obj = nil
    BTagID_TBL.mutex.synchronize{
      if BTagID_TBL[name]
        obj = BTagID_TBL[name]
      else
        (obj = BTagID_TBL[name] = self.allocate).instance_eval{
          @id = name
        }
      end
    }
    bind(*args, &b) if obj && args != []
    obj
  end

  def initialize(*args, &b)
    Tk_BINDTAG_ID.mutex.synchronize{
      # @id = Tk_BINDTAG_ID.join('')
      @id = Tk_BINDTAG_ID.join(TkCore::INTERP._ip_id_)
      Tk_BINDTAG_ID[1].succ!
    }
    BTagID_TBL.mutex.synchronize{
      BTagID_TBL[@id] = self
    }
    bind(*args, &b) if args != []
  end

  ALL = self.new_by_name('all')

  def name
    @id
  end

  def to_eval
    @id
  end

  def inspect
    #Kernel.format "#<TkBindTag: %s>", @id
    '#<TkBindTag: ' + @id + '>'
  end
end


class TkBindTagAll<TkBindTag
  def TkBindTagAll.new(*args, &b)
    $stderr.puts "Warning: TkBindTagALL is obsolete. Use TkBindTag::ALL\n"

    TkBindTag::ALL.bind(*args, &b) if args != []
    TkBindTag::ALL
  end
end


class TkDatabaseClass<TkBindTag
=begin
  def self.new(name, *args, &b)
    BTagID_TBL.mutex.synchronize{
      return BTagID_TBL[name] if BTagID_TBL[name]
    }
    super(name, *args, &b)
  end

  def initialize(name, *args, &b)
    @id = name
    BTagID_TBL.mutex.synchronize{
      BTagID_TBL[@id] = self
    }
    bind(*args, &b) if args != []
  end
=end
  def self.new(name, *args, &b)
    BTagID_TBL.mutex.synchronize{
      if BTagID_TBL[name]
        BTagID_TBL[name]
      else
        BTagID_TBL[name] = self.allocate.instance_eval{
          initialize(name, *args, &b)
          self
        }
      end
    }
  end

  def initialize(name, *args, &b)
    @id = name
    bind(*args, &b) if args != []
  end

  def inspect
    #Kernel.format "#<TkDatabaseClass: %s>", @id
    '#<TkDatabaseClass: ' + @id + '>'
  end
end
