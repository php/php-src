#!/usr/bin/env ruby
###
### mdoc2man - mdoc to man converter
###
### Quick usage:  mdoc2man.rb < mdoc_manpage.8 > man_manpage.8
###
### Ported from Perl by Akinori MUSHA.
###
###  Copyright (c) 2001 University of Illinois Board of Trustees
###  Copyright (c) 2001 Mark D. Roth
###  Copyright (c) 2002, 2003 Akinori MUSHA
###  All rights reserved.
###
###  Redistribution and use in source and binary forms, with or without
###  modification, are permitted provided that the following conditions
###  are met:
###  1. Redistributions of source code must retain the above copyright
###     notice, this list of conditions and the following disclaimer.
###  2. Redistributions in binary form must reproduce the above copyright
###     notice, this list of conditions and the following disclaimer in the
###     documentation and/or other materials provided with the distribution.
###  3. All advertising materials mentioning features or use of this software
###     must display the following acknowledgement:
###     This product includes software developed by the University of
###     Illinois at Urbana, and their contributors.
###  4. The University nor the names of their
###     contributors may be used to endorse or promote products derived from
###     this software without specific prior written permission.
###
###  THIS SOFTWARE IS PROVIDED BY THE TRUSTEES AND CONTRIBUTORS ``AS IS'' AND
###  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
###  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
###  ARE DISCLAIMED.  IN NO EVENT SHALL THE TRUSTEES OR CONTRIBUTORS BE LIABLE
###  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
###  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
###  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
###  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
###  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
###  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
###  SUCH DAMAGE.
###
### $Id$
###

class Mdoc2Man
  ANGLE = 1
  OPTION = 2
  PAREN = 3

  RE_PUNCT = /^[!"'),\.\/:;>\?\]`]$/

  def initialize
    @name = @date = @id = nil
    @refauthors = @reftitle = @refissue = @refdate = @refopt = nil

    @optlist = 0		### 1 = bullet, 2 = enum, 3 = tag, 4 = item
    @oldoptlist = 0
    @nospace = 0		### 0, 1, 2
    @enum = 0
    @synopsis = true
    @reference = false
    @ext = false
    @extopt = false
    @literal = false
  end

  def mdoc2man(i, o)
    i.each { |line|
      if /^\./ !~ line
	o.print line
	o.print ".br\n" if @literal
	next
      end

      line.slice!(0, 1)

      next if /\\"/ =~ line

      line = parse_macro(line) and o.print line
    }

    initialize
  end

  def parse_macro(line)
    words = line.split
    retval = ''

    quote = []
    dl = false

    while word = words.shift
      case word
      when RE_PUNCT
	while q = quote.pop
	  case q
	  when OPTION
	    retval << ']'
	  when PAREN
	    retval << ')'
	  when ANGLE
	    retval << '>'
	  end
	end
	retval << word
	next
      when 'Li', 'Pf'
	@nospace = 1
	next
      when 'Xo'
	@ext = true
	retval << ' ' unless retval.empty? || /[\n ]\z/ =~ retval
	next
      when 'Xc'
	@ext = false
	retval << "\n" unless @extopt
	break
      when 'Bd'
	@literal = true if words[0] == '-literal'
	retval << "\n"
	break
      when 'Ed'
	@literal = false
	break
      when 'Ns'
	@nospace = 1 if @nospace == 0
	retval.chomp!(' ')
	next
      when 'No'
	retval.chomp!(' ')
	retval << words.shift
	next
      when 'Dq'
	retval << '``'
	begin
	  retval << words.shift << ' '
	end until words.empty? || RE_PUNCT =~ words[0]
	retval.chomp!(' ')
	retval << '\'\''
	@nospace = 1 if @nospace == 0 && RE_PUNCT =~ words[0]
	next
      when 'Sq', 'Ql'
	retval << '`' << words.shift << '\''
	@nospace = 1 if @nospace == 0 && RE_PUNCT =~ words[0]
	next
	# when  'Ic'
	#   retval << '\\fB' << words.shift << '\\fP'
	#   next
      when 'Oo'
	#retval << "[\\c\n"
	@extopt = true
	@nospace = 1 if @nospace == 0
	retval << '['
	next
      when 'Oc'
	@extopt = false
	retval << ']'
	next
      when 'Ao'
	@nospace = 1 if @nospace == 0
	retval << '<'
	next
      when 'Ac'
	retval << '>'
	next
      end

      retval << ' ' if @nospace == 0 && !(retval.empty? || /[\n ]\z/ =~ retval)
      @nospace = 0 if @nospace == 1

      case word
      when 'Dd'
	@date = words.join(' ')
	return nil
      when 'Dt'
	if words.size >= 2 && words[1] == '""' &&
	    /^(.*)\(([0-9])\)$/ =~ words[0]
	  words[0] = $1
	  words[1] = $2
	end
	@id = words.join(' ')
	return nil
      when 'Os'
	retval << '.TH ' << @id << ' "' << @date << '" "' <<
	  words.join(' ') << '"'
	break
      when 'Sh'
	retval << '.SH'
	@synopsis = (words[0] == 'SYNOPSIS')
	next
      when 'Xr'
	retval << '\\fB' << words.shift <<
	  '\\fP(' << words.shift << ')' << (words.shift||'')
	break
      when 'Rs'
	@refauthors = []
	@reftitle = ''
	@refissue = ''
	@refdate = ''
	@refopt = ''
	@reference = true
	break
      when 'Re'
	retval << "\n"

	# authors
	while @refauthors.size > 1
	  retval << @refauthors.shift << ', '
	end
	retval << 'and ' unless retval.empty?
	retval << @refauthors.shift

	# title
	retval << ', \\fI' << @reftitle << '\\fP'

	# issue
	retval << ', ' << @refissue unless @refissue.empty?

	# date
	retval << ', ' << @refdate unless @refdate.empty?

	# optional info
	retval << ', ' << @refopt unless @refopt.empty?

	retval << ".\n"

	@reference = false
	break
      when 'An'
	next
      when 'Dl'
	retval << ".nf\n" << '\\&  '
	dl = true
	next
      when 'Ux'
	retval << "UNIX"
	next
      end

      if @reference
	case word
	when '%A'
	  @refauthors.unshift(words.join(' '))
	  break
	when '%T'
	  @reftitle = words.join(' ')
	  @reftitle.sub!(/^"/, '')
	  @reftitle.sub!(/"$/, '')
	  break
	when '%N'
	  @refissue = words.join(' ')
	  break
	when '%D'
	  @refdate = words.join(' ')
	  break
	when '%O'
	  @refopt = words.join(' ')
	  break
	end
      end

      case word
      when 'Nm'
	name = words.empty? ? @name : words.shift
	@name ||= name
	retval << ".br\n" if @synopsis
	retval << "\\fB" << name << "\\fP"
	@nospace = 1 if @nospace == 0 && RE_PUNCT =~ words[0]
	next
      when 'Nd'
	retval << '\\-'
	next
      when 'Fl'
	retval << '\\fB\\-' << words.shift << '\\fP'
	@nospace = 1 if @nospace == 0 && RE_PUNCT =~ words[0]
	next
      when 'Ar'
	retval << '\\fI'
	if words.empty?
	  retval << 'file ...\\fP'
	else
	  retval << words.shift << '\\fP'
	  while words[0] == '|'
	    retval << ' ' << words.shift << ' \\fI' << words.shift << '\\fP'
	  end
	  @nospace = 1 if @nospace == 0 && RE_PUNCT =~ words[0]
	  next
	end
      when 'Cm'
	retval << '\\fB' << words.shift << '\\fP'
	while RE_PUNCT =~ words[0]
	  retval << words.shift
	end
	next
      when 'Op'
	quote << OPTION
	@nospace = 1 if @nospace == 0
	retval << '['
	# words.push(words.pop + ']')
	next
      when 'Aq'
	quote << ANGLE
	@nospace = 1 if @nospace == 0
	retval << '<'
	# words.push(words.pop + '>')
	next
      when 'Pp'
	retval << "\n"
	next
      when 'Ss'
	retval << '.SS'
	next
      end

      if word == 'Pa' && !quote.include?(OPTION)
	retval << '\\fI'
	retval << '\\&' if /^\./ =~ words[0]
	retval << words.shift << '\\fP'
	while RE_PUNCT =~ words[0]
	  retval << words.shift
	end
	# @nospace = 1 if @nospace == 0 && RE_PUNCT =~ words[0]
	next
      end

      case word
      when 'Dv'
	retval << '.BR'
	next
      when 'Em', 'Ev'
	retval << '.IR'
	next
      when 'Pq'
	retval << '('
	@nospace = 1
	quote << PAREN
	next
      when 'Sx', 'Sy'
	retval << '.B ' << words.join(' ')
	break
      when 'Ic'
	retval << '\\fB'
	until words.empty? || RE_PUNCT =~ words[0]
	  case words[0]
	  when 'Op'
	    words.shift
	    retval << '['
	    words.push(words.pop + ']')
	    next
	  when 'Aq'
	    words.shift
	    retval << '<'
	    words.push(words.pop + '>')
	    next
	  when 'Ar'
	    words.shift
	    retval << '\\fI' << words.shift << '\\fP'
	  else
	    retval << words.shift
	  end

	  retval << ' ' if @nospace == 0
	end

	retval.chomp!(' ')
	retval << '\\fP'
	retval << words.shift unless words.empty?
	break
      when 'Bl'
	@oldoptlist = @optlist

	case words[0]
	when '-bullet'
	  @optlist = 1
	when '-enum'
	  @optlist = 2
	  @enum = 0
	when '-tag'
	  @optlist = 3
	when '-item'
	  @optlist = 4
	end

	break
      when 'El'
	@optlist = @oldoptlist
	next
      end

      if @optlist != 0 && word == 'It'
	case @optlist
	when 1
	  # bullets
	  retval << '.IP \\(bu'
	when 2
	  # enum
	  @enum += 1
	  retval << '.IP ' << @enum << '.'
	when 3
	  # tags
	  retval << ".TP\n"
	  case words[0]
	  when 'Pa', 'Ev'
	    words.shift
	    retval << '.B'
	  end
	when 4
	  # item
	  retval << ".IP\n"
	end

	next
      end

      case word
      when 'Sm'
	case words[0]
	when 'off'
	  @nospace = 2
	when 'on'
	  # retval << "\n"
	  @nospace = 0
	end
	words.shift
	next
      end

      retval << word
    end

    return nil if retval == '.'

    retval.sub!(/\A\.([^a-zA-Z])/, "\\1")
    # retval.chomp!(' ')

    while q = quote.pop
      case q
      when OPTION
	retval << ']'
      when PAREN
	retval << ')'
      when ANGLE
	retval << '>'
      end
    end

    # retval << ' ' unless @nospace == 0 || retval.empty? || /\n\z/ =~ retval

    retval << ' ' unless !@ext || @extopt || / $/ =~ retval

    retval << "\n" unless @ext || @extopt || retval.empty? || /\n\z/ =~ retval

    retval << ".fi\n" if dl

    return retval
  end

  def self.mdoc2man(i, o)
    new.mdoc2man(i, o)
  end
end

if $0 == __FILE__
  Mdoc2Man.mdoc2man(ARGF, STDOUT)
end
