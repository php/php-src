require 'test/unit'
require 'tempfile'

class TestRubyMode < Test::Unit::TestCase
  MISCDIR = File.expand_path("../../../misc", __FILE__)
  e = ENV["EMACS"] || "emacs"
  emacs = %W"#{e} -q --no-site-file --batch --load #{MISCDIR}/ruby-mode.el"
  begin
    raise if IO.popen([e, "--version", :err=>[:child, :out]]) {|f| f.read}[/[0-9]+/].to_i < 23
    IO.popen([*emacs, :err=>[:child, :out]]) {|f| f.read}
  rescue
    EMACS = nil
  else
    EMACS = (emacs if $? and $?.success?)
  end
end

class TestRubyMode
  EVAL_OPT = "--eval"
  EXPR_SAVE = "(save-buffer)"
  EXPR_RUBYMODE = "(ruby-mode)"
  EXPR_NOBACKUP = "(progn" \
  " (set (make-local-variable \'backup-inhibited) t)" \
  " (set-buffer-modified-p t)" \
  ")"

  def run_emacs(src, *exprs)
    tmp = Tempfile.new(%w"ruby-mode.test. .rb")
    tmp.puts(src)
    tmp.close
    exprs = exprs.map {|expr| [EVAL_OPT, expr]}.flatten
    exprs.unshift(EVAL_OPT, EXPR_RUBYMODE)
    exprs.unshift(EVAL_OPT, EXPR_NOBACKUP)
    output = IO.popen([*EMACS, tmp.path, *exprs, err:[:child, :out]], "r") {|e| e.read}
    tmp.open
    result = tmp.read
    tmp.close!
    return result, output
  end

  class TestIndent < self
    EXPR_INDENT = "(indent-region (point-min) (point-max))"

    def assert_indent(expected, source, *message)
      if space = expected[/\A\n?(\s*\|)/, 1]
        space = /^#{Regexp.quote(space)}/m
        expected.gsub!(space, '')
        source.gsub!(space, '')
      end
      result, output = run_emacs(source, EXPR_INDENT, EXPR_SAVE)
      assert_match(/^Wrote /, output)
      assert_equal(expected, result, *message)
    end

    def test_simple
      assert_indent('
      |if foo
      |  bar
      |end
      |zot
      |', '
      |if foo
      |bar
      |  end
      |    zot
      |')
    end

    def test_keyword_label
      assert_indent('
      |bar(class: XXX) do
      |  foo
      |end
      |bar
      |', '
      |bar(class: XXX) do
      |     foo
      |  end
      |    bar
      |')
    end

    def test_method_with_question_mark
      assert_indent('
      |if x.is_a?(XXX)
      |  foo
      |end
      |', '
      |if x.is_a?(XXX)
      | foo
      |   end
      |')
    end

    def test_expr_in_regexp
      assert_indent('
      |if /#{foo}/ =~ s
      |  x = 1
      |end
      |', '
      |if /#{foo}/ =~ s
      | x = 1
      |  end
      |')
    end

    def test_singleton_class
      skip("pending")
      assert_indent('
      |class<<bar
      |  foo
      |end
      |', '
      |class<<bar
      |foo
      |   end
      |')
    end

    def test_array_literal
      assert_indent('
      |foo = [
      |  bar
      |]
      |', '
      |foo = [
      | bar
      |  ]
      |')
      assert_indent('
      |foo do
      |  [bar]
      |end
      |', '
      |foo do
      |[bar]
      |  end
      |')
    end

    def test_begin_end
      assert_indent('
      |begin
      |  a[b]
      |end
      |', '
      |begin
      | a[b]
      |  end
      |')
    end

    def test_array_after_paren_and_space
      assert_indent('
      |class A
      |  def foo
      |    foo( [])
      |  end
      |end
      |', '
      |class A
      | def foo
      |foo( [])
      |end
      |  end
      |')
    end
  end
end if TestRubyMode::EMACS
