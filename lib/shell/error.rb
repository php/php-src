#
#   shell/error.rb -
#       $Release Version: 0.7 $
#       $Revision$
#       by Keiju ISHITSUKA(keiju@ruby-lang.org)
#
# --
#
#
#

require "e2mmap"

class Shell
  module Error
    extend Exception2MessageMapper
    def_e2message TypeError, "wrong argument type %s (expected %s)"

    def_exception :DirStackEmpty, "Directory stack empty."
    def_exception :CantDefine, "Can't define method(%s, %s)."
    def_exception :CantApplyMethod, "This method(%s) does not apply to this type(%s)."
    def_exception :CommandNotFound, "Command not found(%s)."
  end
end

