#
# tk/tagfont.rb : control font of tags
#
require 'tk'

module TkTreatTagFont
  def font_configinfo
    @parent.tagfont_configinfo(@id)
  end
#  alias font font_configinfo

  def font_configure(slot)
    @parent.tagfont_configure(@id, slot)
    self
  end

  def latinfont_configure(ltn, keys=nil)
    @parent.latintagfont_configure(@id, ltn, keys)
    self
  end
  alias asciifont_configure latinfont_configure

  def kanjifont_configure(knj, keys=nil)
    @parent.kanjitagfont_configure(@id, ltn, keys)
    self
  end

  def font_copy(win, wintag=nil)
    @parent.tagfont_copy(@id, win, wintag)
    self
  end

  def latinfont_copy(win, wintag=nil)
    @parent.latintagfont_copy(@id, win, wintag)
    self
  end
  alias asciifont_copy latinfont_copy

  def kanjifont_copy(win, wintag=nil)
    @parent.kanjitagfont_copy(@id, win, wintag)
    self
  end
end
