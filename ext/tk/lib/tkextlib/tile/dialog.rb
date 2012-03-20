#
#  ttk::dialog  (tile-0.7+)
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'
require 'tkextlib/tile.rb'

module Tk
  module Tile
    class Dialog < TkWindow
    end
  end
end

begin
  TkPackage.require('ttk::dialog') # this may be required.
rescue RuntimeError
  # ignore
end

class Tk::Tile::Dialog
  TkCommandNames = ['::ttk::dialog'.freeze].freeze

  def self.show(*args)
    dialog = self.new(*args)
    dialog.show
    [dialog.status, dialog.value]
  end
  def self.display(*args)
    self.show(*args)
  end

  def self.define_dialog_type(name, keys)
    Tk.tk_call('::ttk::dialog::define', name, keys)
    name
  end

  def self.style(*args)
    ['Dialog', *(args.map!{|a| _get_eval_string(a)})].join('.')
  end

  #########################

  def initialize(keys={})
    @keys = _symbolkey2str(keys)
    super(*args)
  end

  def create_self(keys)
    # dummy
  end
  private :create_self

  def show
    tk_call(self.class::TkCommandNames[0], @path, *hash_kv(@keys))
  end
  alias display show

  def client_frame
    window(tk_call_without_enc('::ttk::dialog::clientframe', @path))
  end

  def cget_strict(slot)
    @keys[slot.to_s]
  end
  def cget(slot)
    @keys[slot.to_s]
  end
=begin
  def cget(slot)
    unless TkConfigMethod.__IGNORE_UNKNOWN_CONFIGURE_OPTION__
      cget_strict(slot)
    else
      cget_strict(slot) rescue nil
    end
  end
=end

  def configure(slot, value=None)
    if slot.kind_of?(Hash)
      slot.each{|k, v| configure(k, v)}
    else
      slot = slot.to_s
      value = _symbolkey2str(value) if value.kind_of?(Hash)
      if value && value != None
        @keys[slot] = value
      else
        @keys.delete(slot)
      end
    end
    self
  end

  def configinfo(slot = nil)
    if slot
      slot = slot.to_s
      [ slot, nil, nil, nil, @keys[slot] ]
    else
      @keys.collect{|k, v| [ k, nil, nil, nil, v ] }
    end
  end
end
