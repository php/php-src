##
# RDoc::CrossReference is a reusable way to create cross references for names.

class RDoc::CrossReference

  ##
  # Regular expression to match class references
  #
  # 1. There can be a '\\' in front of text to suppress the cross-reference
  # 2. There can be a '::' in front of class names to reference from the
  #    top-level namespace.
  # 3. The method can be followed by parenthesis (not recommended)

  CLASS_REGEXP_STR = '\\\\?((?:\:{2})?[A-Z]\w*(?:\:\:\w+)*)'

  ##
  # Regular expression to match method references.
  #
  # See CLASS_REGEXP_STR

  METHOD_REGEXP_STR = '([a-z]\w*[!?=]?)(?:\([\w.+*/=<>-]*\))?'

  ##
  # Regular expressions matching text that should potentially have
  # cross-reference links generated are passed to add_special.  Note that
  # these expressions are meant to pick up text for which cross-references
  # have been suppressed, since the suppression characters are removed by the
  # code that is triggered.

  CROSSREF_REGEXP = /(
                      # A::B::C.meth
                      #{CLASS_REGEXP_STR}(?:[.#]|::)#{METHOD_REGEXP_STR}

                      # Stand-alone method (preceded by a #)
                      | \\?\##{METHOD_REGEXP_STR}

                      # Stand-alone method (preceded by ::)
                      | ::#{METHOD_REGEXP_STR}

                      # A::B::C
                      # The stuff after CLASS_REGEXP_STR is a
                      # nasty hack.  CLASS_REGEXP_STR unfortunately matches
                      # words like dog and cat (these are legal "class"
                      # names in Fortran 95).  When a word is flagged as a
                      # potential cross-reference, limitations in the markup
                      # engine suppress other processing, such as typesetting.
                      # This is particularly noticeable for contractions.
                      # In order that words like "can't" not
                      # be flagged as potential cross-references, only
                      # flag potential class cross-references if the character
                      # after the cross-reference is a space, sentence
                      # punctuation, tag start character, or attribute
                      # marker.
                      | #{CLASS_REGEXP_STR}(?=[\s\)\.\?\!\,\;<\000]|\z)

                      # Things that look like filenames
                      # The key thing is that there must be at least
                      # one special character (period, slash, or
                      # underscore).
                      | (?:\.\.\/)*[-\/\w]+[_\/\.][-\w\/\.]+

                      # Things that have markup suppressed
                      # Don't process things like '\<' in \<tt>, though.
                      # TODO: including < is a hack, not very satisfying.
                      | \\[^\s<]
                      )/x

  ##
  # Version of CROSSREF_REGEXP used when <tt>--hyperlink-all</tt> is specified.

  ALL_CROSSREF_REGEXP = /(
                      # A::B::C.meth
                      #{CLASS_REGEXP_STR}(?:[.#]|::)#{METHOD_REGEXP_STR}

                      # Stand-alone method
                      | \\?#{METHOD_REGEXP_STR}

                      # A::B::C
                      | #{CLASS_REGEXP_STR}(?=[\s\)\.\?\!\,\;<\000]|\z)

                      # Things that look like filenames
                      | (?:\.\.\/)*[-\/\w]+[_\/\.][-\w\/\.]+

                      # Things that have markup suppressed
                      | \\[^\s<]
                      )/x

  attr_accessor :seen

  ##
  # Allows cross-references to be created based on the given +context+
  # (RDoc::Context).

  def initialize context
    @context = context

    @seen = {}
  end

  ##
  # Returns a reference to +name+.
  #
  # If the reference is found and +name+ is not documented +text+ will be
  # returned.  If +name+ is escaped +name+ is returned.  If +name+ is not
  # found +text+ is returned.

  def resolve name, text
    return @seen[name] if @seen.include? name

    # Find class, module, or method in class or module.
    #
    # Do not, however, use an if/elsif/else chain to do so.  Instead, test
    # each possible pattern until one matches.  The reason for this is that a
    # string like "YAML.txt" could be the txt() class method of class YAML (in
    # which case it would match the first pattern, which splits the string
    # into container and method components and looks up both) or a filename
    # (in which case it would match the last pattern, which just checks
    # whether the string as a whole is a known symbol).

    if /#{CLASS_REGEXP_STR}([.#]|::)#{METHOD_REGEXP_STR}/o =~ name then
      type = $2
      type = '' if type == '.'  # will find either #method or ::method
      method = "#{type}#{$3}"
      container = @context.find_symbol_module($1)
    elsif /^([.#]|::)#{METHOD_REGEXP_STR}/o =~ name then
      type = $1
      type = '' if type == '.'
      method = "#{type}#{$2}"
      container = @context
    else
      container = nil
    end

    if container then
      ref = container.find_local_symbol method

      unless ref || RDoc::TopLevel === container then
        ref = container.find_ancestor_local_symbol method
      end
    end

    ref = case name
          when /^\\(#{CLASS_REGEXP_STR})$/o then
            ref = @context.find_symbol $1
          else
            ref = @context.find_symbol name
          end unless ref

    ref = nil if RDoc::Alias === ref # external alias: can't link to it

    out = if name == '\\' then
            name
          elsif name =~ /^\\/ then
            # we remove the \ only in front of what we know:
            # other backslashes are treated later, only outside of <tt>
            ref ? $' : name
          elsif ref then
            if ref.display? then
              ref
            else
              text
            end
          else
            text
          end

    @seen[name] = out

    out
  end

end

