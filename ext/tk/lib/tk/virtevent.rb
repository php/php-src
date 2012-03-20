#
#   tk/virtevent.rb : treats virtual events
#                     1998/07/16 by Hidetoshi Nagai <nagai@ai.kyutech.ac.jp>
#
require 'tk'

class TkVirtualEvent<TkObject
  extend Tk

  TkCommandNames = ['event'.freeze].freeze

  (TkVirtualEventID = ["VirtEvent".freeze, TkUtil.untrust("00000")]).instance_eval{
    @mutex = Mutex.new
    def mutex; @mutex; end
    freeze
  }

  TkVirtualEventTBL = TkCore::INTERP.create_table

  TkCore::INTERP.init_ip_env{
    TkVirtualEventTBL.mutex.synchronize{ TkVirtualEventTBL.clear }
  }

  class PreDefVirtEvent<self
    def self.new(event, *sequences)
      if event =~ /^<(<.*>)>$/
        event = $1
      elsif event !~ /^<.*>$/
        event = '<' + event + '>'
      end
      TkVirtualEvent::TkVirtualEventTBL.mutex.synchronize{
        if TkVirtualEvent::TkVirtualEventTBL.has_key?(event)
          TkVirtualEvent::TkVirtualEventTBL[event]
        else
          # super(event, *sequences)
          (obj = self.allocate).instance_eval{
            initialize(event, *sequences)
            TkVirtualEvent::TkVirtualEventTBL[@id] = self
          }
        end
      }
    end

    def initialize(event, *sequences)
      @path = @id = event
      _add_sequences(sequences)
    end
  end

  def TkVirtualEvent.getobj(event)
    obj = nil
    TkVirtualEventTBL.mutex.synchronize{
      obj = TkVirtualEventTBL[event]
    }
    if obj
      obj
    else
      if tk_call_without_enc('event', 'info').index("<#{event}>")
        PreDefVirtEvent.new(event)
      else
        fail ArgumentError, "undefined virtual event '<#{event}>'"
      end
    end
  end

  def TkVirtualEvent.info
    tk_call_without_enc('event', 'info').split(/\s+/).collect!{|seq|
      TkVirtualEvent.getobj(seq[1..-2])
    }
  end

  def initialize(*sequences)
    TkVirtualEventID.mutex.synchronize{
      # @path = @id = '<' + TkVirtualEventID.join('') + '>'
      @path = @id = '<' + TkVirtualEventID.join(TkCore::INTERP._ip_id_) + '>'
      TkVirtualEventID[1].succ!
    }
    _add_sequences(sequences)
  end

  def _add_sequences(seq_ary)
    unless seq_ary.empty?
      tk_call_without_enc('event', 'add', "<#{@id}>",
                          *(seq_ary.collect{|seq|
                              "<#{tk_event_sequence(seq)}>"
                            }) )
    end
    self
  end
  private :_add_sequences

  def add(*sequences)
    if sequences != []
      _add_sequences(sequences)
      TkVirtualEventTBL.mutex.synchronize{
        TkVirtualEventTBL[@id] = self
      }
    end
    self
  end

  def delete(*sequences)
    if sequences.empty?
      tk_call_without_enc('event', 'delete', "<#{@id}>")
      TkVirtualEventTBL.mutex.synchronize{
        TkVirtualEventTBL.delete(@id)
      }
    else
      tk_call_without_enc('event', 'delete', "<#{@id}>",
                          *(sequences.collect{|seq|
                              "<#{tk_event_sequence(seq)}>"
                            }) )
      if tk_call_without_enc('event','info',"<#{@id}>").empty?
        TkVirtualEventTBL.mutex.synchronize{
          TkVirtualEventTBL.delete(@id)
        }
      end
    end
    self
  end

  def info
    tk_call_without_enc('event','info',"<#{@id}>").split(/\s+/).collect!{|seq|
      lst = seq.scan(/<*[^<>]+>*/).collect!{|subseq|
        case (subseq)
        when /^<<[^<>]+>>$/
          TkVirtualEvent.getobj(subseq[1..-2])
        when /^<[^<>]+>$/
          subseq[1..-2]
        else
          subseq.split('')
        end
      }.flatten
      (lst.size == 1) ? lst[0] : lst
    }
  end
end

TkNamedVirtualEvent = TkVirtualEvent::PreDefVirtEvent
