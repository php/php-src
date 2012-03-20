#
#  ttk::spinbox widget  (Tcl/Tk 8.6b1 or later)
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'
require 'tkextlib/tile.rb'

module Tk
  module Tile
    class TSpinbox < Tk::Tile::TEntry
    end
    Spinbox = TSpinbox
  end
end

class Tk::Tile::TSpinbox < Tk::Tile::TEntry
  include Tk::Tile::TileWidget

  if Tk::Tile::USE_TTK_NAMESPACE
    TkCommandNames = ['::ttk::spinbox'.freeze].freeze
  else
    TkCommandNames = ['::tspinbox'.freeze].freeze
  end
  WidgetClassName = 'TSpinbox'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  class SpinCommand < TkValidateCommand
    class ValidateArgs < TkUtil::CallbackSubst
      KEY_TBL = [
        [ ?d, ?s, :direction ],
        [ ?s, ?e, :current ],
        [ ?W, ?w, :widget ],
        nil
      ]

      PROC_TBL = [
        [ ?s, TkComm.method(:string) ],
        [ ?w, TkComm.method(:window) ],

        [ ?e, proc{|val|
            #enc = Tk.encoding
            enc = ((Tk.encoding)? Tk.encoding : Tk.encoding_system)
            if enc
              Tk.fromUTF8(TkComm::string(val), enc)
            else
              TkComm::string(val)
            end
          }
        ],

        nil
      ]

      _setup_subst_table(KEY_TBL, PROC_TBL);

      def self.ret_val(val)
        (val)? '1': '0'
      end
    end

    def self._config_keys
      ['command']
    end
  end

  def __validation_class_list
    super() << SpinCommand
  end

  Tk::ValidateConfigure.__def_validcmd(binding, SpinCommand)

  def __boolval_optkeys
    super() << 'wrap'
  end
  private :__boolval_optkeys

  def __strval_optkeys
    super() << 'buttonbackground' << 'format'
  end
  private :__strval_optkeys

  def __listval_optkeys
    super() << 'values'
  end
  private :__listval_optkeys

  def self.style(*args)
    [self::WidgetClassName, *(args.map!{|a| _get_eval_string(a)})].join('.')
  end

  def current
    number(tk_send_without_enc('current'))
  end
  def current=(idx)
    tk_send('current', idx)
  end

  def set(val)
    tk_send('set', val)
  end

  alias identify ttk_identify
end

#Tk.__set_toplevel_aliases__(:Ttk, Tk::Tile::Spinbox, :TkSpinbox)
Tk.__set_loaded_toplevel_aliases__('tkextlib/tile/tspinbox.rb',
                                   :Ttk, Tk::Tile::Spinbox, :TkSpinbox)
