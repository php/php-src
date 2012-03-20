#!/usr/bin/env ruby

# example:
# ./enc-emoji4unicode.rb emoji4unicode.xml > ../enc/trans/emoji-exchange-tbl.rb

require 'rexml/document'
require File.expand_path("../transcode-tblgen", __FILE__)

class EmojiTable
  VERBOSE_MODE = false

  def initialize(xml_path)
    @doc = REXML::Document.new File.open(xml_path)
    @kddi_undoc = make_kddi_undoc_map()
  end

  def conversion(from_carrier, to_carrier, &block)
    REXML::XPath.each(@doc.root, '//e') do |e|
      from = e.attribute(from_carrier.downcase).to_s
      to = e.attribute(to_carrier.downcase).to_s
      text_fallback = e.attribute('text_fallback').to_s
      name = e.attribute('name').to_s
      if from =~ /^(?:\*|\+)(.+)$/ # proposed or unified
        from = $1
      end
      if from.empty? || from !~ /^[0-9A-F]+$/
        # do nothing
      else
        from_utf8 = [from.hex].pack("U").unpack("H*").first
        if to =~ /^(?:&gt;|\*)?([0-9A-F\+]+)$/
          str_to = $1
          if str_to =~ /^\+/ # unicode "proposed" begins at "+"
            proposal = true
            str_to.sub!(/^\+/, '')
          else
            proposal = false
          end
          tos = str_to.split('+')
          to_utf8 = tos.map(&:hex).pack("U*").unpack("H*").first
          comment = "[%s] U+%X -> %s" % [name, from.hex, tos.map{|c| "U+%X"%c.hex}.join(' ')]
          block.call(:from => from_utf8,
                     :to => to_utf8,
                     :comment => comment,
                     :fallback => false,
                     :proposal => proposal)
        elsif to.empty?
          if text_fallback.empty?
            comment = "[%s] U+%X -> U+3013 (GETA)" % [name, from.hex]
            block.call(:from => from_utf8,
                       :to => "\u{3013}".unpack("H*").first,
                       :comment => comment, # geta
                       :fallback => true,
                       :proposal => false)
          else
            to_utf8 = text_fallback.unpack("H*").first
            comment = %([%s] U+%X -> "%s") % [name, from.hex, text_fallback]
            block.call(:from => from_utf8,
                       :to => to_utf8,
                       :comment => comment,
                       :fallback => true,
                       :proposal => false)
          end
        else
          raise "something wrong: %s -> %s" % [from, to]
        end
      end
    end
  end

  def generate(io, from_carrier, to_carrier)
    from_encoding = (from_carrier == "Unicode") ? "UTF-8" : "UTF8-"+from_carrier
    to_encoding   = (to_carrier == "Unicode" )  ? "UTF-8" : "UTF8-"+to_carrier
      io.puts "EMOJI_EXCHANGE_TBL['#{from_encoding}']['#{to_encoding}'] = ["
      io.puts "  # for documented codepoints" if from_carrier == "KDDI"
      self.conversion(from_carrier, to_carrier) do |params|
        from, to = params[:from], %Q{"#{params[:to]}"}
        to = ":undef" if params[:fallback] || params[:proposal]
        io.puts %{  ["#{from}", #{to}], # #{params[:comment]}}
      end
      if from_carrier == "KDDI"
        io.puts "  # for undocumented codepoints"
        self.conversion(from_carrier, to_carrier) do |params|
          from, to = params[:from], %Q{"#{params[:to]}"}
          to = ":undef" if params[:fallback] || params[:proposal]
          unicode = utf8_to_ucs(from)
          undoc = ucs_to_utf8(@kddi_undoc[unicode])
          io.puts %{  ["#{undoc}", #{to}], # #{params[:comment]}}
        end
      end
      io.puts "]"
      io.puts
  end

  private

  def utf8_to_ucs(cp)
    return [cp].pack("H*").unpack("U*").first
  end

  def ucs_to_utf8(cp)
    return [cp].pack("U*").unpack("H*").first
  end

  def make_kddi_undoc_map()
    pub_to_sjis = citrus_decode_mapsrc(
      "mskanji", 2, "UCS/EMOJI_SHIFT_JIS-KDDI").sort_by{|u, s| s}
    sjis_to_undoc = citrus_decode_mapsrc(
      "mskanji", 2, "EMOJI_SHIFT_JIS-KDDI-UNDOC/UCS").sort_by{|s, u| s}
    return pub_to_sjis.zip(sjis_to_undoc).inject({}) {|h, rec|
      raise "no match sjis codepoint" if rec[0][1] != rec[1][0]
      h[rec[0][0]] = rec[1][1]
      next h
    }
  end
end

if ARGV.empty?
  puts "usage: #$0 [emoji4unicode.xml]"
  exit 1
end
$srcdir = File.expand_path("../../enc/trans", __FILE__)
emoji_table = EmojiTable.new(ARGV[0])

companies = %w(DoCoMo KDDI SoftBank Unicode)

io = STDOUT
io.puts "EMOJI_EXCHANGE_TBL = Hash.new{|h,k| h[k] = {}}"
companies.each do |from_company|
  companies.each do |to_company|
    next if from_company == to_company
    emoji_table.generate(io, from_company, to_company)
  end
end
