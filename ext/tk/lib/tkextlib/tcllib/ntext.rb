#
#  tkextlib/tcllib/ntext.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * Ntext bindtag
#

require 'tk'
require 'tkextlib/tcllib.rb'

# TkPackage.require('ntext', '0.81')
TkPackage.require('ntext')

module Tk::Tcllib
  Ntext = TkBindTag.new_by_name('Ntext')
end

# variables
Tk::Tcllib::Ntext.instance_eval{
  # boolean
  @classicAnchor = TkVarAccess.new('::ntext::classicAnchor')
  @classicExtras  = TkVarAccess.new('::ntext::classicExtras')
  @classicMouseSelect = TkVarAccess.new('::ntext::classicMouseSelect')
  @classicWordBreak = TkVarAccess.new('::ntext::classicWordBreak')
  @classicWrap = TkVarAccess.new('::ntext::classicWrap')
  @overwrite = TkVarAccess.new('::ntext::overwrite')

  # regexp
  @newWrapRegexp = TkVarAccess.new('::ntext::newWrapRegexp')

  # variables (advanced use)
  @tcl_match_wordBreakAfter = TkVarAccess.new('::ntext::tcl_match_wordBreakAfter')
  @tcl_match_wordBreakBefore = TkVarAccess.new('::ntext::tcl_match_wordBreakBefore')
  @tcl_match_endOfWord = TkVarAccess.new('::ntext::tcl_match_endOfWord')
  @tcl_match_startOfNextWord = TkVarAccess.new('::ntext::tcl_match_startOfNextWord')
  @tcl_match_startOfPreviousWord = TkVarAccess.new('::ntext::tcl_match_startOfPreviousWord')
}

class << Tk::Tcllib::Ntext
  def wrapIndent(txt, *args)
    TK.tk_call('::next::wrapIndent', txt, *args)
  end

  def initializeMatchPatterns
    TK.tk_call('::next::initializeMatchPatterns')
    self
  end

  def createMatchPatterns(*args)
    TK.tk_call('::next::createMatchPatterns', *args)
    self
  end

  # functions (advanced use)
  #ntext::new_wordBreakAfter
  #ntext::new_wordBreakBefore
  #ntext::new_endOfWord
  #ntext::new_startOfNextWord
  #ntext::new_startOfPreviousWord

  # accessor
  def classicAnchor
    @classicAnchor.bool
  end
  def classicAnchor=(mode)
    @classicAnchor.bool = mode
  end

  def classicExtras
    @classicExtras.bool
  end
  def classicExtras=(mode)
    @classicExtras.bool = mode
  end

  def classicMouseSelect
    @classicMouseSelect.bool
  end
  def classicMouseSelect=(mode)
    @classicMouseSelect.bool = mode
  end

  def classicWordBreak
    @classicWordBreak.bool
  end
  def classicWordBreak=(mode)
    @classicWordBreak.bool = mode
  end

  def classicWrap
    @classicWrap.bool
  end
  def classicWrap=(mode)
    @classicWrap.bool = mode
  end

  def overwrite
    @overwrite.bool
  end
  def overwrite=(mode)
    @classic.bool = mode
  end

  def newWrapRegexp
    @newWrapRegexp.value
  end
  def newWrapRegexp=(val)
    @newWrapRegexp.value = val
  end

  def tcl_match_wordBreakAfter
    @tcl_match_wordBreakAfter.value
  end
  def tcl_match_wordBreakAfter=(val)
    @tcl_match_wordBreakAfter.value = val
  end

  def tcl_match_wordBreakBefore
    @tcl_match_wordBreakBefore.value
  end
  def tcl_match_wordBreakBefore=(val)
    @tcl_match_wordBreakBefore.value = val
  end

  def tcl_match_endOfWord
    @tcl_match_endOfWord.value
  end
  def tcl_match_endOfWord=(val)
    @tcl_match_endOfWord.value = val
  end

  def tcl_match_startOfNextWord
    @tcl_match_startOfNextWord.value
  end
  def tcl_match_startOfNextWord=(val)
    @tcl_match_startOfNextWord.value = val
  end

  def tcl_match_startOfPreviousWord
    @tcl_match_startOfPreviousWord.value
  end
  def tcl_match_startOfPreviousWord=(val)
    @tcl_match_startOfPreviousWord.value = val
  end
end
