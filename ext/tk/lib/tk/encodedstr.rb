#
# tk/encodedstr.rb : Tk::EncodedString class
#
require 'tk'

###########################################
#  string with Tcl's encoding
###########################################
module Tk
  class EncodedString < String
    Encoding = nil

    def self.subst_utf_backslash(str)
      # str.gsub(/\\u([0-9A-Fa-f]{1,4})/){[$1.hex].pack('U')}
      TclTkLib._subst_UTF_backslash(str)
    end
    def self.utf_backslash(str)
      self.subst_utf_backslash(str)
    end

    def self.subst_tk_backslash(str)
      TclTkLib._subst_Tcl_backslash(str)
    end

    def self.utf_to_backslash_sequence(str)
      str.unpack('U*').collect{|c|
        if c <= 0xFF  # ascii character
          c.chr
        else
          format('\u%X', c)
        end
      }.join('')
    end
    def self.utf_to_backslash(str)
      self.utf_to_backslash_sequence(str)
    end

    def self.to_backslash_sequence(str)
      str.unpack('U*').collect{|c|
        if c <= 0x1F  # control character
          case c
          when 0x07; '\a'
          when 0x08; '\b'
          when 0x09; '\t'
          when 0x0a; '\n'
          when 0x0b; '\v'
          when 0x0c; '\f'
          when 0x0d; '\r'
          else
            format('\x%02X', c)
          end
        elsif c <= 0xFF  # ascii character
          c.chr
        else
          format('\u%X', c)
        end
      }.join('')
    end

    def self.new_with_utf_backslash(str, enc = nil)
      self.new('', enc).replace(self.subst_utf_backslash(str))
    end

    def self.new_without_utf_backslash(str, enc = nil)
      self.new('', enc).replace(str)
    end

    def initialize(str, enc = nil)
      super(str)
      # @encoding = ( enc ||
      #              ((self.class::Encoding)?
      #                  self.class::Encoding : Tk.encoding_system) )
      enc ||= (self.class::Encoding)?
                         self.class::Encoding :
                         ((Tk.encoding)? Tk.encoding : Tk.encoding_system)
      if TkCore::WITH_ENCODING
        unless encobj = Tk::Encoding::ENCODING_TABLE.get_obj(enc)
          fail ArgumentError, "unsupported Tk encoding '#{enc}'"
        end
        self.force_encoding(encobj)
      else
        @encoding = enc
      end
    end

    if TkCore::WITH_ENCODING
      alias encoding_obj encoding
      alias __encoding   encoding
      def encoding
        Tk::Encoding::ENCODING_TABLE.get_name(super())
      end
    else
      def encoding
        @encoding
      end
      alias encoding_obj encoding
    end

    if TkCore::WITH_ENCODING
      # wrapper methods for compatibility
      alias __instance_variable_get instance_variable_get
      alias __instance_variable_set instance_variable_set
      alias __instance_eval         instance_eval
      alias __instance_variables    instance_variables

      def instance_variable_get(key)
        if (key.to_s == '@encoding')
          self.encoding
        else
          super(key)
        end
      end

      def instance_variable_set(key, value)
        if (key.to_s == '@encoding')
          if value
            self.force_encoding(value)
          else
            self.force_encoding(Tk::Encoding::UNKNOWN)
          end
          value
        else
          super(key, value)
        end
      end

      def instance_eval(*args, &b)
        old_enc = @encoding = self.encoding

        ret = super(*args, &b)

        if @encoding
          if @encoding != old_enc
            # modified by user
            self.force_encoding(@encoding)
          end
          remove_instance_variable(:@encoding)
        else
          begin
            remove_instance_variable(:@encoding)
            # user sets to nil -> use current default
            self.force_encoding(Tk.encoding)
          rescue NameError
            # removed by user -> ignore, because user don't use @encoding
          end
        end
        ret
      end
    end

    def instance_variables
      ret = super()
      ret << :@encoding  # fake !!
      ret
    end
  end
  # def Tk.EncodedString(str, enc = nil)
  #   Tk::EncodedString.new(str, enc)
  # end

  ##################################

  class BinaryString < EncodedString
    Encoding = 'binary'.freeze
  end
  # def Tk.BinaryString(str)
  #   Tk::BinaryString.new(str)
  # end

  ##################################

  class UTF8_String < EncodedString
    Encoding = 'utf-8'.freeze
    def self.new(str)
      super(self.subst_utf_backslash(str))
    end

    def to_backslash_sequence
      Tk::EncodedString.utf_to_backslash_sequence(self)
    end
    alias to_backslash to_backslash_sequence
  end
  # def Tk.UTF8_String(str)
  #   Tk::UTF8_String.new(str)
  # end

end
