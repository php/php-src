#
# tk/itemfont.rb : control font of widget items
#
require 'tk'

module TkItemFontOptkeys
  def __item_font_optkeys(id)
    # maybe need to override
    ['font']
  end
  private :__item_font_optkeys
end

module TkTreatItemFont
  include TkItemFontOptkeys

  def __item_pathname(id)
    # maybe need to override
    [self.path, id].join(';')
  end
  private :__item_pathname

  ################################################

  def tagfont_configinfo(tagOrId, key = nil)
    optkeys = __item_font_optkeys(tagid(tagOrId))
    if key && !optkeys.find{|opt| opt.to_s == key.to_s}
      fail ArgumentError, "unknown font option name `#{key}'"
    end

    win, tag = __item_pathname(tagid(tagOrId)).split(';')

    if key
      pathname = [win, tag, key].join(';')
      TkFont.used_on(pathname) ||
        TkFont.init_widget_font(pathname,
                                *(__item_confinfo_cmd(tagid(tagOrId))))
    elsif optkeys.size == 1
      pathname = [win, tag, optkeys[0]].join(';')
      TkFont.used_on(pathname) ||
        TkFont.init_widget_font(pathname,
                                *(__item_confinfo_cmd(tagid(tagOrId))))
    else
      fonts = {}
      optkeys.each{|key|
        key = key.to_s
        pathname = [win, tag, key].join(';')
        fonts[key] =
          TkFont.used_on(pathname) ||
          TkFont.init_widget_font(pathname,
                                  *(__item_confinfo_cmd(tagid(tagOrId))))
      }
      fonts
    end
  end
  alias tagfontobj tagfont_configinfo

  def tagfont_configure(tagOrId, slot)
    pathname = __item_pathname(tagid(tagOrId))

    slot = _symbolkey2str(slot)

    __item_font_optkeys(tagid(tagOrId)).each{|optkey|
      optkey = optkey.to_s
      l_optkey = 'latin' << optkey
      a_optkey = 'ascii' << optkey
      k_optkey = 'kanji' << optkey

      if slot.key?(optkey)
        fnt = slot.delete(optkey)
        if fnt.kind_of?(TkFont)
          slot.delete(l_optkey)
          slot.delete(a_optkey)
          slot.delete(k_optkey)

          fnt.call_font_configure([pathname, optkey],
                                  *(__item_config_cmd(tagid(tagOrId)) << {}))
          next
        else
          if fnt
            if (slot.key?(l_optkey) ||
                slot.key?(a_optkey) ||
                slot.key?(k_optkey))
              fnt = TkFont.new(fnt)

              lfnt = slot.delete(l_optkey)
              lfnt = slot.delete(a_optkey) if slot.key?(a_optkey)
              kfnt = slot.delete(k_optkey)

              fnt.latin_replace(lfnt) if lfnt
              fnt.kanji_replace(kfnt) if kfnt

              fnt.call_font_configure([pathname, optkey],
                                      *(__item_config_cmd(tagid(tagOrId)) << {}))
              next
            else
              fnt = hash_kv(fnt) if fnt.kind_of?(Hash)
              unless TkItemConfigMethod.__IGNORE_UNKNOWN_CONFIGURE_OPTION__
                tk_call(*(__item_config_cmd(tagid(tagOrId)) << "-#{optkey}" << fnt))
              else
                begin
                  tk_call(*(__item_config_cmd(tagid(tagOrId)) << "-#{optkey}" << fnt))
                rescue => e
                  # ignore
                end
              end
            end
          end
          next
        end
      end

      lfnt = slot.delete(l_optkey)
      lfnt = slot.delete(a_optkey) if slot.key?(a_optkey)
      kfnt = slot.delete(k_optkey)

      if lfnt && kfnt
        TkFont.new(lfnt, kfnt).call_font_configure([pathname, optkey],
                                                   *(__item_config_cmd(tagid(tagOrId)) << {}))
      elsif lfnt
        latintagfont_configure([lfnt, optkey])
      elsif kfnt
        kanjitagfont_configure([kfnt, optkey])
      end
    }

    # configure other (without font) options
    tk_call(*(__item_config_cmd(tagid(tagOrId)).concat(hash_kv(slot)))) if slot != {}
    self
  end

  def latintagfont_configure(tagOrId, ltn, keys=nil)
    if ltn.kind_of?(Array)
      key = ltn[1]
      ltn = ltn[0]
    else
      key = nil
    end

    optkeys = __item_font_optkeys(tagid(tagOrId))
    if key && !optkeys.find{|opt| opt.to_s == key.to_s}
      fail ArgumentError, "unknown font option name `#{key}'"
    end

    win, tag = __item_pathname(tagid(tagOrId)).split(';')

    optkeys = [key] if key

    optkeys.each{|optkey|
      optkey = optkey.to_s

      pathname = [win, tag, optkey].join(';')

      if (fobj = TkFont.used_on(pathname))
        fobj = TkFont.new(fobj) # create a new TkFont object
      elsif Tk::JAPANIZED_TK
        fobj = fontobj          # create a new TkFont object
      else
        ltn = hash_kv(ltn) if ltn.kind_of?(Hash)
        unless TkItemConfigMethod.__IGNORE_UNKNOWN_CONFIGURE_OPTION__
          tk_call(*(__item_config_cmd(tagid(tagOrId)) << "-#{optkey}" << ltn))
        else
          begin
            tk_call(*(__item_config_cmd(tagid(tagOrId)) << "-#{optkey}" << ltn))
          rescue => e
            # ignore
          end
        end
        next
      end

      if fobj.kind_of?(TkFont)
        if ltn.kind_of?(TkFont)
          conf = {}
          ltn.latin_configinfo.each{|key,val| conf[key] = val}
          if keys
            fobj.latin_configure(conf.update(keys))
          else
            fobj.latin_configure(conf)
          end
        else
          fobj.latin_replace(ltn)
        end
      end

      fobj.call_font_configure([pathname, optkey], *(__item_config_cmd(tagid(tagOrId)) << {}))
    }
    self
  end
  alias asciitagfont_configure latintagfont_configure

  def kanjitagfont_configure(tagOrId, knj, keys=nil)
    if knj.kind_of?(Array)
      key = knj[1]
      knj = knj[0]
    else
      key = nil
    end

    optkeys = __item_font_optkeys(tagid(tagOrId))
    if key && !optkeys.find{|opt| opt.to_s == key.to_s}
      fail ArgumentError, "unknown font option name `#{key}'"
    end

    win, tag = __item_pathname(tagid(tagOrId)).split(';')

    optkeys = [key] if key

    optkeys.each{|optkey|
      optkey = optkey.to_s

      pathname = [win, tag, optkey].join(';')

      if (fobj = TkFont.used_on(pathname))
        fobj = TkFont.new(fobj) # create a new TkFont object
      elsif Tk::JAPANIZED_TK
        fobj = fontobj          # create a new TkFont object
      else
        knj = hash_kv(knj) if knj.kind_of?(Hash)
        unless TkItemConfigMethod.__IGNORE_UNKNOWN_CONFIGURE_OPTION__
          tk_call(*(__item_config_cmd(tagid(tagOrId)) << "-#{optkey}" << knj))
        else
          begin
            tk_call(*(__item_config_cmd(tagid(tagOrId)) << "-#{optkey}" << knj))
          rescue => e
            # ignore
          end
        end
        next
      end

      if fobj.kind_of?(TkFont)
        if knj.kind_of?(TkFont)
          conf = {}
          knj.kanji_configinfo.each{|key,val| conf[key] = val}
          if keys
            fobj.kanji_configure(conf.update(keys))
          else
            fobj.kanji_configure(conf)
          end
        else
          fobj.kanji_replace(knj)
        end
      end

      fobj.call_font_configure([pathname, optkey], *(__item_config_cmd(tagid(tagOrId)) << {}))
    }
    self
  end

  def tagfont_copy(tagOrId, win, wintag=nil, winkey=nil, targetkey=nil)
    if wintag
      if winkey
        fnt = win.tagfontobj(wintag, winkey).dup
      else
        fnt = win.tagfontobj(wintag).dup
      end
    else
      if winkey
        fnt = win.fontobj(winkey).dup
      else
        fnt = win.fontobj.dup
      end
    end

    if targetkey
      fnt.call_font_configure([__item_pathname(tagid(tagOrId)), targetkey],
                              *(__item_config_cmd(tagid(tagOrId)) << {}))
    else
      fnt.call_font_configure(__item_pathname(tagid(tagOrId)),
                              *(__item_config_cmd(tagid(tagOrId)) << {}))
    end
    self
  end


  def latintagfont_copy(tagOrId, win, wintag=nil, winkey=nil, targetkey=nil)
    if targetkey
      fontobj(targetkey).dup.call_font_configure([__item_pathname(tagid(tagOrId)), targetkey],
                                                 *(__item_config_cmd(tagid(tagOrId)) << {}))
    else
      fontobj.dup.call_font_configure(__item_pathname(tagid(tagOrId)),
                                      *(__item_config_cmd(tagid(tagOrId)) << {}))
    end

    if wintag
      if winkey
        fontobj.latin_replace(win.tagfontobj(wintag, winkey).latin_font_id)
      else
        fontobj.latin_replace(win.tagfontobj(wintag).latin_font_id)
      end
    else
      if winkey
        fontobj.latin_replace(win.fontobj(winkey).latin_font_id)
      else
        fontobj.latin_replace(win.fontobj.latin_font_id)
      end
    end
    self
  end
  alias asciitagfont_copy latintagfont_copy

  def kanjifont_copy(tagOrId, win, wintag=nil, winkey=nil, targetkey=nil)
    if targetkey
      fontobj(targetkey).dup.call_font_configure([__item_pathname(tagid(tagOrId)), targetkey],
                                                 *(__item_config_cmd(tagid(tagOrId)) << {}))
    else
        fontobj.dup.call_font_configure(__item_pathname(tagid(tagOrId)),
                                        *(__item_config_cmd(tagid(tagOrId)) << {}))
    end

    if wintag
      if winkey
        fontobj.kanji_replace(win.tagfontobj(wintag, winkey).kanji_font_id)
      else
        fontobj.kanji_replace(win.tagfontobj(wintag).kanji_font_id)
      end
    else
      if winkey
        fontobj.kanji_replace(win.fontobj(winkey).kanji_font_id)
      else
        fontobj.kanji_replace(win.fontobj.kanji_font_id)
      end
    end
    self
  end
end
