#
# tk/kinput.rb : control kinput
#
require 'tk'

module TkKinput
  include Tk
  extend Tk

  TkCommandNames = [
    'kinput_start'.freeze,
    'kinput_send_spot'.freeze,
    'kanjiInput'.freeze
  ].freeze

  def TkKinput.start(win, style=None)
    tk_call('kinput_start', win, style)
  end
  def kinput_start(style=None)
    TkKinput.start(self, style)
  end

  def TkKinput.send_spot(win)
    tk_call('kinput_send_spot', win)
  end
  def kinput_send_spot
    TkKinput.send_spot(self)
  end

  def TkKinput.input_start(win, keys=nil)
    tk_call('kanjiInput', 'start', win, *hash_kv(keys))
  end
  def kanji_input_start(keys=nil)
    TkKinput.input_start(self, keys)
  end

  def TkKinput.attribute_config(win, slot, value=None)
    if slot.kind_of? Hash
      tk_call('kanjiInput', 'attribute', win, *hash_kv(slot))
    else
      tk_call('kanjiInput', 'attribute', win, "-#{slot}", value)
    end
  end
  def kinput_attribute_config(slot, value=None)
    TkKinput.attribute_config(self, slot, value)
  end

  def TkKinput.attribute_info(win, slot=nil)
    if slot
      conf = tk_split_list(tk_call('kanjiInput', 'attribute',
                                   win, "-#{slot}"))
      conf[0] = conf[0][1..-1]
      conf
    else
      tk_split_list(tk_call('kanjiInput', 'attribute', win)).collect{|conf|
        conf[0] = conf[0][1..-1]
        conf
      }
    end
  end
  def kinput_attribute_info(slot=nil)
    TkKinput.attribute_info(self, slot)
  end

  def TkKinput.input_end(win)
    tk_call('kanjiInput', 'end', win)
  end
  def kanji_input_end
    TkKinput.input_end(self)
  end
end
