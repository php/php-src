require 'minitest/spec'
require 'benchmark'

MiniTest::Unit.autorun

describe Benchmark do
  BENCH_FOR_TIMES_UPTO = lambda do |x|
    n = 1000
    tf = x.report("for:")   { for _ in 1..n; '1'; end }
    tt = x.report("times:") { n.times do   ; '1'; end }
    tu = x.report("upto:")  { 1.upto(n) do ; '1'; end }
    [tf+tt+tu, (tf+tt+tu)/3]
  end

  BENCH_FOR_TIMES_UPTO_NO_LABEL = lambda do |x|
    n = 1000
    x.report { for _ in 1..n; '1'; end }
    x.report { n.times do   ; '1'; end }
    x.report { 1.upto(n) do ; '1'; end }
  end

  LABELS = %w[first second third]

  def bench(type = :bm, *args, &block)
    if block
      Benchmark.send(type, *args, &block)
    else
      Benchmark.send(type, *args) do |x|
        LABELS.each { |label|
          x.report(label) {}
        }
      end
    end
  end

  def capture_output
    capture_io { yield }.first.gsub(/[ \-]\d\.\d{6}/, ' --time--')
  end

  def capture_bench_output(type, *args, &block)
    capture_output { bench(type, *args, &block) }
  end

  describe 'Tms' do
    it 'outputs nicely' do
      Benchmark::Tms.new.to_s.must_equal            "  0.000000   0.000000   0.000000 (  0.000000)\n"
      Benchmark::Tms.new(1,2,3,4,5).to_s.must_equal "  1.000000   2.000000  10.000000 (  5.000000)\n"
      Benchmark::Tms.new(1,2,3,4,5,'label').format('%u %y %U %Y %t %r %n').must_equal \
        "1.000000 2.000000 3.000000 4.000000 10.000000 (5.000000) label"
      Benchmark::Tms.new(1).format('%u %.3f', 2).must_equal "1.000000 2.000"
      Benchmark::Tms.new(100, 150, 0, 0, 200).to_s.must_equal \
        "100.000000 150.000000 250.000000 (200.000000)\n"
    end

    it 'wont modify the format String given' do
      format = "format %u"
      Benchmark::Tms.new.format(format)
      format.must_equal "format %u"
    end
  end

  describe 'benchmark' do
    it 'makes extra calcultations with an Array at the end of the benchmark and show the result' do
      capture_bench_output(:benchmark,
        Benchmark::CAPTION, 7,
        Benchmark::FORMAT, ">total:", ">avg:",
        &BENCH_FOR_TIMES_UPTO).must_equal BENCHMARK_OUTPUT_WITH_TOTAL_AVG
    end
  end

  describe 'bm' do
    it "returns an Array of the times with the labels" do
      [:bm, :bmbm].each do |meth|
        capture_io do
          results = bench(meth)
          results.must_be_instance_of Array
          results.size.must_equal LABELS.size
          results.zip(LABELS).each { |tms, label|
            tms.must_be_instance_of Benchmark::Tms
            tms.label.must_equal label
          }
        end
      end
    end

    it 'correctly output when the label width is given' do
      capture_bench_output(:bm, 6).must_equal BM_OUTPUT
    end

    it 'correctly output when no label is given' do
      capture_bench_output(:bm, &BENCH_FOR_TIMES_UPTO_NO_LABEL).must_equal BM_OUTPUT_NO_LABEL
    end

    it 'can make extra calcultations with an array at the end of the benchmark' do
      capture_bench_output(:bm, 7, ">total:", ">avg:",
        &BENCH_FOR_TIMES_UPTO).must_equal BENCHMARK_OUTPUT_WITH_TOTAL_AVG
    end
  end

  describe 'bmbm' do
    it 'correctly guess the label width even when not given' do
      capture_bench_output(:bmbm).must_equal BMBM_OUTPUT
    end

    it 'correctly output when the label width is given (bmbm ignore it, but it is a frequent mistake)' do
      capture_bench_output(:bmbm, 6).must_equal BMBM_OUTPUT
    end
  end

  describe 'Report' do
    describe '#item' do
      it 'shows the title, even if not a string' do
        capture_bench_output(:bm) { |x| x.report(:title) {} }.must_include 'title'
        capture_bench_output(:bmbm) { |x| x.report(:title) {} }.must_include 'title'
      end
    end
  end

  describe 'Bugs' do
    it '[ruby-dev:40906] can add in-place the time of execution of the block given' do
      t = Benchmark::Tms.new
      t.real.must_equal 0
      t.add! { sleep 0.1 }
      t.real.wont_equal 0
    end
  end
end

BM_OUTPUT = <<BENCH
             user     system      total        real
first    --time--   --time--   --time-- (  --time--)
second   --time--   --time--   --time-- (  --time--)
third    --time--   --time--   --time-- (  --time--)
BENCH

BM_OUTPUT_NO_LABEL = <<BENCH
       user     system      total        real
   --time--   --time--   --time-- (  --time--)
   --time--   --time--   --time-- (  --time--)
   --time--   --time--   --time-- (  --time--)
BENCH

BMBM_OUTPUT = <<BENCH
Rehearsal ------------------------------------------
first    --time--   --time--   --time-- (  --time--)
second   --time--   --time--   --time-- (  --time--)
third    --time--   --time--   --time-- (  --time--)
--------------------------------- total: --time--sec

             user     system      total        real
first    --time--   --time--   --time-- (  --time--)
second   --time--   --time--   --time-- (  --time--)
third    --time--   --time--   --time-- (  --time--)
BENCH

BENCHMARK_OUTPUT_WITH_TOTAL_AVG = <<BENCH
              user     system      total        real
for:      --time--   --time--   --time-- (  --time--)
times:    --time--   --time--   --time-- (  --time--)
upto:     --time--   --time--   --time-- (  --time--)
>total:   --time--   --time--   --time-- (  --time--)
>avg:     --time--   --time--   --time-- (  --time--)
BENCH
