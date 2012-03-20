# == sentence library
#
# = Features
#
# * syntax based sentences generation
# * sentence operations such as substitution.
#
# = Example
#
# Some arithmetic expressions using "+", "-", "*" and "/" are generated as follows.
#
#  require 'sentence'
#  Sentence.each({
#    :exp => [["num"],
#             [:exp, "+", :exp],
#             [:exp, "-", :exp],
#             [:exp, "*", :exp],
#             [:exp, "/", :exp]]
#  }, :exp, 2) {|sent| p sent }
#  #=>
#  #<Sentence: "num">
#  #<Sentence: ("num") "+" ("num")>
#  #<Sentence: ("num") "+" (("num") "+" ("num"))>
#  #<Sentence: ("num") "+" (("num") "-" ("num"))>
#  #<Sentence: ("num") "+" (("num") "*" ("num"))>
#  #<Sentence: ("num") "+" (("num") "/" ("num"))>
#  #<Sentence: (("num") "+" ("num")) "+" ("num")>
#  ...
#
# Sentence.each takes 3 arguments.
# The first argument is the syntax for the expressions.
# The second argument, :exp, is a generating nonterminal.
# The third argument, 2, limits derivation to restrict results finitely.
#
# Some arithmetic expressions including parenthesis can be generated as follows.
#
#  syntax = {
#    :factor => [["n"],
#                ["(", :exp, ")"]],
#    :term => [[:factor],
#              [:term, "*", :factor],
#              [:term, "/", :factor]],
#    :exp => [[:term],
#             [:exp, "+", :term],
#             [:exp, "-", :term]]
#  }
#  Sentence.each(syntax, :exp, 2) {|sent| p sent }
#  #=>
#  #<Sentence: (("n"))>
#  #<Sentence: (("(" ((("n"))) ")"))>
#  #<Sentence: (("(" ((("(" ((("n"))) ")"))) ")"))>
#  #<Sentence: (("(" (((("n")) "*" ("n"))) ")"))>
#  #<Sentence: (("(" (((("n")) "/" ("n"))) ")"))>
#  #<Sentence: (("(" (((("n"))) "+" (("n"))) ")"))>
#  #<Sentence: (("(" (((("n"))) "-" (("n"))) ")"))>
#  #<Sentence: ((("n")) "*" ("n"))>
#  #<Sentence: ((("n")) "*" ("(" ((("n"))) ")"))>
#  ...
#
#  Sentence#to_s can be used to concatenate strings
#  in a sentence:
#
#  Sentence.each(syntax, :exp, 2) {|sent| p sent.to_s }
#  #=>
#  "n"
#  "(n)"
#  "((n))"
#  "(n*n)"
#  "(n/n)"
#  "(n+n)"
#  "(n-n)"
#  "n*n"
#  "n*(n)"
#  ...
#

# Sentence() instantiates a sentence object.
#
#  Sentence("foo", "bar")
#  #=> #<Sentence: "foo" "bar">
#
#  Sentence("foo", ["bar", "baz"])
#  #=> #<Sentence: "foo" ("bar" "baz")>
#
def Sentence(*ary)
  Sentence.new(ary)
end

# Sentence class represents a tree with string leaves.
#
class Sentence
  # _ary_ represents a tree.
  # It should be a possibly nested array which contains strings.
  #
  # Note that _ary_ is not copied.
  # Don't modify _ary_ after the sentence object is instantiated.
  #
  #  Sentence.new(["a", "pen"])
  #  #<Sentence: "a" "pen">
  #
  #  Sentence.new(["I", "have", ["a", "pen"]])
  #  #<Sentence: "I" "have" ("a" "pen")>
  #
  def initialize(ary)
    @sent = ary
  end

  # returns a string which is concatenation of all strings.
  # No separator is used.
  #
  #  Sentence("2", "+", "3").to_s
  #  "2+3"
  #
  #  Sentence("2", "+", ["3", "*", "5"]).to_s
  #  "2+3*5"
  #
  def to_s
    @sent.join('')
  end

  # returns a string which is concatenation of all strings separated by _sep_.
  # If _sep_ is not given, single space is used.
  #
  #  Sentence("I", "have", ["a", "pen"]).join
  #  "I have a pen"
  #
  #  Sentence("I", "have", ["a", "pen"]).join("/")
  #  "I/have/a/pen"
  #
  #  Sentence("a", [], "b").join("/")
  #  "a/b"
  #
  def join(sep=' ')
    @sent.flatten.join(sep)
  end

  # returns a tree as a nested array.
  #
  # Note that the result is not copied.
  # Don't modify the result.
  #
  #  Sentence(["foo", "bar"], "baz").to_a
  #  #=> [["foo", "bar"], "baz"]
  #
  def to_a
    @sent
  end

  # returns <i>i</i>th element as a sentence or string.
  #
  #  s = Sentence(["foo", "bar"], "baz")
  #  s    #=> #<Sentence: ("foo" "bar") "baz">
  #  s[0] #=> #<Sentence: "foo" "bar">
  #  s[1] #=> "baz"
  #
  def [](i)
    e = @sent[i]
    e.respond_to?(:to_ary) ? Sentence.new(e) : e
  end

  # returns the number of top level elements.
  #
  #  Sentence.new(%w[foo bar]).length
  #  #=> 2
  #
  #  Sentence(%w[2 * 7], "+", %w[3 * 5]).length
  #  #=> 3
  #
  def length
    @sent.length
  end

  # iterates over children.
  #
  #  Sentence(%w[2 * 7], "+", %w[3 * 5]).each {|v| p v }
  #  #=>
  #  #<Sentence: "2" "*" "7">
  #  "+"
  #  #<Sentence: "3" "*" "5">
  #
  def each # :yield: element
    @sent.each_index {|i|
      yield self[i]
    }
  end
  include Enumerable

  def inspect
    "#<#{self.class}: #{inner_inspect(@sent, '')}>"
  end

  # :stopdoc:
  def inner_inspect(ary, r)
    first = true
    ary.each {|obj|
      r << ' ' if !first
      first = false
      if obj.respond_to? :to_ary
        r << '('
        inner_inspect(obj, r)
        r << ')'
      else
        r << obj.inspect
      end
    }
    r
  end
  # :startdoc:

  # returns new sentence object which
  # _target_ is substituted by the block.
  #
  # Sentence#subst invokes <tt>_target_ === _string_</tt> for each
  # string in the sentence.
  # The strings which === returns true are substituted by the block.
  # The block is invoked with the substituting string.
  #
  #  Sentence.new(%w[2 + 3]).subst("+") { "*" }
  #  #<Sentence: "2" "*" "3">
  #
  #  Sentence.new(%w[2 + 3]).subst(/\A\d+\z/) {|s| ((s.to_i)*2).to_s }
  #  #=> #<Sentence: "4" "+" "6">
  #
  def subst(target, &b) # :yield: string
    Sentence.new(subst_rec(@sent, target, &b))
  end

  # :stopdoc:
  def subst_rec(obj, target, &b)
    if obj.respond_to? :to_ary
      a = []
      obj.each {|e| a << subst_rec(e, target, &b) }
      a
    elsif target === obj
      yield obj
    else
      obj
    end
  end
  # :startdoc:

  # find a subsentence and return it.
  # The block is invoked for each subsentence in preorder manner.
  # The first subsentence which the block returns true is returned.
  #
  #  Sentence(%w[2 * 7], "+", %w[3 * 5]).find_subtree {|s| s[1] == "*" }
  #  #=> #<Sentence: "2" "*" "7">
  #
  def find_subtree(&b) # :yield: sentence
    find_subtree_rec(@sent, &b)
  end

  # :stopdoc:
  def find_subtree_rec(obj, &b)
    if obj.respond_to? :to_ary
      s = Sentence.new(obj)
      if b.call s
        return s
      else
        obj.each {|e|
          r = find_subtree_rec(e, &b)
          return r if r
        }
      end
    end
    nil
  end
  # :startdoc:

  # returns a new sentence object which expands according to the condition
  # given by the block.
  #
  # The block is invoked for each subsentence.
  # The subsentences which the block returns true are
  # expanded into parent.
  #
  #  s = Sentence(%w[2 * 7], "+", %w[3 * 5])
  #  #=> #<Sentence: ("2" "*" "7") "+" ("3" "*" "5")>
  #
  #  s.expand { true }
  #  #=> #<Sentence: "2" "*" "7" "+" "3" "*" "5">
  #
  #  s.expand {|s| s[0] == "3" }
  #  #=> #<Sentence: (("2" "*" "7") "+" "3" "*" "5")>
  #
  def expand(&b) # :yield: sentence
    Sentence.new(expand_rec(@sent, &b))
  end

  # :stopdoc:
  def expand_rec(obj, r=[], &b)
    if obj.respond_to? :to_ary
      obj.each {|o|
        s = Sentence.new(o)
        if b.call s
          expand_rec(o, r, &b)
        else
          a = []
          expand_rec(o, a, &b)
          r << a
        end
      }
    else
      r << obj
    end
    r
  end
  # :startdoc:

  # Sentence.each generates sentences
  # by deriving the start symbol _sym_ using _syntax_.
  # The derivation is restricted by an positive integer _limit_ to
  # avoid infinite generation.
  #
  # Sentence.each yields the block with a generated sentence.
  #
  #  Sentence.each({
  #    :exp => [["n"],
  #             [:exp, "+", :exp],
  #             [:exp, "*", :exp]]
  #    }, :exp, 1) {|sent| p sent }
  #  #=>
  #  #<Sentence: "n">
  #  #<Sentence: ("n") "+" ("n")>
  #  #<Sentence: ("n") "*" ("n")>
  #
  #  Sentence.each({
  #    :exp => [["n"],
  #             [:exp, "+", :exp],
  #             [:exp, "*", :exp]]
  #    }, :exp, 2) {|sent| p sent }
  #  #=>
  #  #<Sentence: "n">
  #  #<Sentence: ("n") "+" ("n")>
  #  #<Sentence: ("n") "+" (("n") "+" ("n"))>
  #  #<Sentence: ("n") "+" (("n") "*" ("n"))>
  #  #<Sentence: (("n") "+" ("n")) "+" ("n")>
  #  #<Sentence: (("n") "*" ("n")) "+" ("n")>
  #  #<Sentence: ("n") "*" ("n")>
  #  #<Sentence: ("n") "*" (("n") "+" ("n"))>
  #  #<Sentence: ("n") "*" (("n") "*" ("n"))>
  #  #<Sentence: (("n") "+" ("n")) "*" ("n")>
  #  #<Sentence: (("n") "*" ("n")) "*" ("n")>
  #
  def Sentence.each(syntax, sym, limit)
    Gen.new(syntax).each_tree(sym, limit) {|tree|
      yield Sentence.new(tree)
    }
  end

  # Sentence.expand_syntax returns an expanded syntax:
  # * No rule derives to empty sequence
  # * Underivable rule simplified
  # * No channel rule
  # * Symbols which has zero or one choices are not appered in rhs.
  #
  # Note that the rules which can derive empty and non-empty
  # sequences are modified to derive only non-empty sequences.
  #
  #  Sentence.expand_syntax({
  #    :underivable1 => [],
  #    :underivable2 => [[:underivable1]],
  #    :underivable3 => [[:underivable3]],
  #    :empty_only1 => [[]],
  #    :empty_only2 => [[:just_empty1, :just_empty1]],
  #    :empty_or_not => [[], ["foo"]],
  #    :empty_or_not_2 => [[:empty_or_not, :empty_or_not]],
  #    :empty_or_not_3 => [[:empty_or_not, :empty_or_not, :empty_or_not]],
  #    :empty_or_not_4 => [[:empty_or_not_2, :empty_or_not_2]],
  #    :channel1 => [[:channeled_data]],
  #    :channeled_data => [["a", "b"], ["c", "d"]],
  #    :single_choice => [["single", "choice"]],
  #    :single_choice_2 => [[:single_choice, :single_choice]],
  #  })
  #  #=>
  #  {
  #   :underivable1=>[], # underivable rules are simplified to [].
  #   :underivable2=>[],
  #   :underivable3=>[],
  #   :empty_only1=>[], # derivation to empty sequence are removed.
  #   :empty_only2=>[],
  #   :empty_or_not=>[["foo"]], # empty sequences are removed too.
  #   :empty_or_not_2=>[["foo"], ["foo", "foo"]],
  #   :empty_or_not_3=>[["foo"], ["foo", "foo"], ["foo", "foo", "foo"]],
  #   :empty_or_not_4=> [["foo"], ["foo", "foo"], [:empty_or_not_2, :empty_or_not_2]],
  #   :channel1=>[["a", "b"], ["c", "d"]], # channel rules are removed.
  #   :channeled_data=>[["a", "b"], ["c", "d"]],
  #   :single_choice=>[["single", "choice"]], # single choice rules are expanded.
  #   :single_choice_2=>[["single", "choice", "single", "choice"]],
  #  }
  #
  #  Sentence.expand_syntax({
  #    :factor => [["n"],
  #                ["(", :exp, ")"]],
  #    :term => [[:factor],
  #              [:term, "*", :factor],
  #              [:term, "/", :factor]],
  #    :exp => [[:term],
  #             [:exp, "+", :term],
  #             [:exp, "-", :term]]
  #  })
  #  #=>
  #  {:exp=> [["n"],
  #           ["(", :exp, ")"],
  #           [:exp, "+", :term],
  #           [:exp, "-", :term],
  #           [:term, "*", :factor],
  #           [:term, "/", :factor]],
  #   :factor=> [["n"],
  #              ["(", :exp, ")"]],
  #   :term=> [["n"],
  #            ["(", :exp, ")"],
  #            [:term, "*", :factor],
  #            [:term, "/", :factor]]
  #  }
  #
  def Sentence.expand_syntax(syntax)
    Sentence::Gen.expand_syntax(syntax)
  end

  # :stopdoc:
  class Gen
    def Gen.each_tree(syntax, sym, limit, &b)
      Gen.new(syntax).each_tree(sym, limit, &b)
    end

    def Gen.each_string(syntax, sym, limit, &b)
      Gen.new(syntax).each_string(sym, limit, &b)
    end

    def initialize(syntax)
      @syntax = syntax
    end

    def self.expand_syntax(syntax)
      syntax = simplify_underivable_rules(syntax)
      syntax = simplify_emptyonly_rules(syntax)
      syntax = make_rules_no_empseq(syntax)
      syntax = expand_channel_rules(syntax)

      syntax = expand_noalt_rules(syntax)
      syntax = reorder_rules(syntax)
    end

    def self.simplify_underivable_rules(syntax)
      deribable_syms = {}
      changed = true
      while changed
        changed = false
        syntax.each {|sym, rules|
          next if deribable_syms[sym]
          rules.each {|rhs|
            if rhs.all? {|e| String === e || deribable_syms[e] }
              deribable_syms[sym] = true
              changed = true
              break
            end
          }
        }
      end
      result = {}
      syntax.each {|sym, rules|
        if deribable_syms[sym]
          rules2 = []
          rules.each {|rhs|
            rules2 << rhs if rhs.all? {|e| String === e || deribable_syms[e] }
          }
          result[sym] = rules2.uniq
        else
          result[sym] = []
        end
      }
      result
    end

    def self.simplify_emptyonly_rules(syntax)
      justempty_syms = {}
      changed = true
      while changed
        changed = false
        syntax.each {|sym, rules|
          next if justempty_syms[sym]
          if !rules.empty? && rules.all? {|rhs| rhs.all? {|e| justempty_syms[e] } }
            justempty_syms[sym] = true
            changed = true
          end
        }
      end
      result = {}
      syntax.each {|sym, rules|
        result[sym] = rules.map {|rhs| rhs.reject {|e| justempty_syms[e] } }.uniq
      }
      result
    end

    def self.expand_emptyable_syms(rhs, emptyable_syms)
      if rhs.empty?
        yield []
      else
        first = rhs[0]
        rest = rhs[1..-1]
        if emptyable_syms[first]
          expand_emptyable_syms(rest, emptyable_syms) {|rhs2|
            yield [first] + rhs2
            yield rhs2
          }
        else
          expand_emptyable_syms(rest, emptyable_syms) {|rhs2|
            yield [first] + rhs2
          }
        end
      end
    end

    def self.make_rules_no_empseq(syntax)
      emptyable_syms = {}
      changed = true
      while changed
        changed = false
        syntax.each {|sym, rules|
          next if emptyable_syms[sym]
          rules.each {|rhs|
            if rhs.all? {|e| emptyable_syms[e] }
              emptyable_syms[sym] = true
              changed = true
              break
            end
          }
        }
      end
      result = {}
      syntax.each {|sym, rules|
        rules2 = []
        rules.each {|rhs|
          expand_emptyable_syms(rhs, emptyable_syms) {|rhs2|
            next if rhs2.empty?
            rules2 << rhs2
          }
        }
        result[sym] = rules2.uniq
      }
      result
    end

    def self.expand_channel_rules(syntax)
      channel_rules = {}
      syntax.each {|sym, rules|
        channel_rules[sym] = {sym=>true}
        rules.each {|rhs|
          if rhs.length == 1 && Symbol === rhs[0]
            channel_rules[sym][rhs[0]] = true
          end
        }
      }
      changed = true
      while changed
        changed = false
        channel_rules.each {|sym, set|
          n1 = set.size
          set.keys.each {|s|
            set.update(channel_rules[s])
          }
          n2 = set.size
          changed = true if n1 < n2
        }
      end
      result = {}
      syntax.each {|sym, rules|
        rules2 = []
        channel_rules[sym].each_key {|s|
          syntax[s].each {|rhs|
            unless rhs.length == 1 && Symbol === rhs[0]
              rules2 << rhs
            end
          }
        }
        result[sym] = rules2.uniq
      }
      result
    end

    def self.expand_noalt_rules(syntax)
      noalt_syms = {}
      syntax.each {|sym, rules|
        if rules.length == 1
          noalt_syms[sym] = true
        end
      }
      result = {}
      syntax.each {|sym, rules|
        rules2 = []
        rules.each {|rhs|
          rhs2 = []
          rhs.each {|e|
            if noalt_syms[e]
              rhs2.concat syntax[e][0]
            else
              rhs2 << e
            end
          }
          rules2 << rhs2
        }
        result[sym] = rules2.uniq
      }
      result
    end

    def self.reorder_rules(syntax)
      result = {}
      syntax.each {|sym, rules|
        result[sym] = rules.sort_by {|rhs|
          [rhs.find_all {|e| Symbol === e }.length, rhs.length]
        }
      }
      result
    end

    def each_tree(sym, limit)
      generate_from_sym(sym, limit) {|_, tree|
        yield tree
      }
      nil
    end

    def each_string(sym, limit)
      generate_from_sym(sym, limit) {|_, tree|
        yield [tree].join('')
      }
      nil
    end

    def generate_from_sym(sym, limit, &b)
      return if limit < 0
      if String === sym
        yield limit, sym
      else
        rules = @syntax[sym]
        raise "undefined rule: #{sym}" if !rules
        rules.each {|rhs|
          if rhs.length == 1 || rules.length == 1
            limit1 = limit
          else
            limit1 = limit-1
          end
          generate_from_rhs(rhs, limit1, &b)
        }
      end
      nil
    end

    def generate_from_rhs(rhs, limit)
      return if limit < 0
      if rhs.empty?
        yield limit, []
      else
        generate_from_sym(rhs[0], limit) {|limit1, child|
          generate_from_rhs(rhs[1..-1], limit1) {|limit2, arr|
            yield limit2, [child, *arr]
          }
        }
      end
      nil
    end
  end
  # :startdoc:

end

