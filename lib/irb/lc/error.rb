#
#   irb/lc/error.rb -
#   	$Release Version: 0.9.6$
#   	$Revision$
#   	by Keiju ISHITSUKA(keiju@ruby-lang.org)
#
# --
#
#
#
require "e2mmap"

module IRB

  # exceptions
  extend Exception2MessageMapper
  def_exception :UnrecognizedSwitch, "Unrecognized switch: %s"
  def_exception :NotImplementedError, "Need to define `%s'"
  def_exception :CantReturnToNormalMode, "Can't return to normal mode."
  def_exception :IllegalParameter, "Invalid parameter(%s)."
  def_exception :IrbAlreadyDead, "Irb is already dead."
  def_exception :IrbSwitchedToCurrentThread, "Switched to current thread."
  def_exception :NoSuchJob, "No such job(%s)."
  def_exception :CantShiftToMultiIrbMode, "Can't shift to multi irb mode."
  def_exception :CantChangeBinding, "Can't change binding to (%s)."
  def_exception :UndefinedPromptMode, "Undefined prompt mode(%s)."

end

