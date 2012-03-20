= minitest/{unit,spec,mock,benchmark}

home :: https://github.com/seattlerb/minitest
rdoc :: http://docs.seattlerb.org/minitest
vim  :: https://github.com/sunaku/vim-ruby-minitest

== DESCRIPTION:

minitest provides a complete suite of testing facilities supporting
TDD, BDD, mocking, and benchmarking.

    "I had a class with Jim Weirich on testing last week and we were
     allowed to choose our testing frameworks. Kirk Haines and I were
     paired up and we cracked open the code for a few test
     frameworks...

     I MUST say that mintiest is *very* readable / understandable
     compared to the 'other two' options we looked at. Nicely done and
     thank you for helping us keep our mental sanity."

    -- Wayne E. Seguin

minitest/unit is a small and incredibly fast unit testing framework.
It provides a rich set of assertions to make your tests clean and
readable.

minitest/spec is a functionally complete spec engine. It hooks onto
minitest/unit and seamlessly bridges test assertions over to spec
expectations.

minitest/benchmark is an awesome way to assert the performance of your
algorithms in a repeatable manner. Now you can assert that your newb
co-worker doesn't replace your linear algorithm with an exponential
one!

minitest/mock by Steven Baker, is a beautifully tiny mock object
framework.

minitest/pride shows pride in testing and adds coloring to your test
output. I guess it is an example of how to write IO pipes too. :P

minitest/unit is meant to have a clean implementation for language
implementors that need a minimal set of methods to bootstrap a working
test suite. For example, there is no magic involved for test-case
discovery.

    "Again, I can’t praise enough the idea of a testing/specing
     framework that I can actually read in full in one sitting!"

    -- Piotr Szotkowski

== FEATURES/PROBLEMS:

* minitest/autorun - the easy and explicit way to run all your tests.
* minitest/unit - a very fast, simple, and clean test system.
* minitest/spec - a very fast, simple, and clean spec system.
* minitest/mock - a simple and clean mock system.
* minitest/benchmark - an awesome way to assert your algorithm's performance.
* minitest/pride - show your pride in testing!
* Incredibly small and fast runner, but no bells and whistles.

== RATIONALE:

See design_rationale.rb to see how specs and tests work in minitest.

== SYNOPSIS:

Given that you'd like to test the following class:

  class Meme
    def i_can_has_cheezburger?
      "OHAI!"
    end

    def will_it_blend?
      "YES!"
    end
  end

=== Unit tests

  require 'minitest/autorun'

  class TestMeme < MiniTest::Unit::TestCase
    def setup
      @meme = Meme.new
    end

    def test_that_kitty_can_eat
      assert_equal "OHAI!", @meme.i_can_has_cheezburger?
    end

    def test_that_it_will_not_blend
      refute_match /^no/i, @meme.will_it_blend?
    end
  end

=== Specs

  require 'minitest/autorun'

  describe Meme do
    before do
      @meme = Meme.new
    end

    describe "when asked about cheeseburgers" do
      it "must respond positively" do
        @meme.i_can_has_cheezburger?.must_equal "OHAI!"
      end
    end

    describe "when asked about blending possibilities" do
      it "won't say no" do
        @meme.will_it_blend?.wont_match /^no/i
      end
    end
  end

=== Benchmarks

Add benchmarks to your regular unit tests. If the unit tests fail, the
benchmarks won't run.

  # optionally run benchmarks, good for CI-only work!
  require 'minitest/benchmark' if ENV["BENCH"]

  class TestMeme < MiniTest::Unit::TestCase
    # Override self.bench_range or default range is [1, 10, 100, 1_000, 10_000]
    def bench_my_algorithm
      assert_performance_linear 0.9999 do |n| # n is a range value
        @obj.my_algorithm(n)
      end
    end
  end

Or add them to your specs. If you make benchmarks optional, you'll
need to wrap your benchmarks in a conditional since the methods won't
be defined.

  describe Meme do
    if ENV["BENCH"] then
      bench_performance_linear "my_algorithm", 0.9999 do |n|
        100.times do
          @obj.my_algorithm(n)
        end
      end
    end
  end

outputs something like:

  # Running benchmarks:

  TestBlah	100	1000	10000
  bench_my_algorithm	 0.006167	 0.079279	 0.786993
  bench_other_algorithm	 0.061679	 0.792797	 7.869932

Output is tab-delimited to make it easy to paste into a spreadsheet.

=== Mocks

  class MemeAsker
    def initialize(meme)
      @meme = meme
    end

    def ask(question)
      method = question.tr(" ","_") + "?"
      @meme.send(method)
    end
  end

  require 'minitest/autorun'

  describe MemeAsker do
    before do
      @meme = MiniTest::Mock.new
      @meme_asker = MemeAsker.new @meme
    end

    describe "#ask" do
      describe "when passed an unpunctuated question" do
        it "should invoke the appropriate predicate method on the meme" do
          @meme.expect :will_it_blend?, :return_value
          @meme_asker.ask "will it blend"
          @meme.verify
        end
      end
    end
  end

=== Customizable Test Runner Types:

MiniTest::Unit.runner=(runner) provides an easy way of creating custom
test runners for specialized needs. Justin Weiss provides the
following real-world example to create an alternative to regular
fixture loading:

  class MiniTestWithHooks::Unit < MiniTest::Unit
    def before_suites
    end

    def after_suites
    end

    def _run_suites(suites, type)
      begin
        before_suites
        super(suites, type)
      ensure
        after_suites
      end
    end

    def _run_suite(suite, type)
      begin
        suite.before_suite
        super(suite, type)
      ensure
        suite.after_suite
      end
    end
  end

  module MiniTestWithTransactions
    class Unit < MiniTestWithHooks::Unit
      include TestSetupHelper

      def before_suites
        super
        setup_nested_transactions
        # load any data we want available for all tests
      end

      def after_suites
        teardown_nested_transactions
        super
      end
    end
  end

  MiniTest::Unit.runner = MiniTestWithTransactions::Unit.new

== REQUIREMENTS:

* Ruby 1.8, maybe even 1.6 or lower. No magic is involved.

== INSTALL:

  sudo gem install minitest

On 1.9, you already have it. To get newer candy you can still install
the gem, but you'll need to activate the gem explicitly to use it:

  require 'rubygems'
  gem 'minitest' # ensures you're using the gem, and not the built in MT
  require 'minitest/autorun'
  
  # ... usual testing stuffs ...

== LICENSE:

(The MIT License)

Copyright (c) Ryan Davis, seattle.rb

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
'Software'), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
