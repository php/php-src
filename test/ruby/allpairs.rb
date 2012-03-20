module AllPairs
  module_function

  def make_prime(v)
    return 2 if v < 2
    ary = [true] * (v*2)
    2.upto(Math.sqrt(ary.length).ceil) {|i|
      return i if ary[i] && v <= i
      (i*2).step(ary.length, i) {|j|
        ary[j] = false
      }
    }
    v.upto(ary.length-1) {|i|
      return i if ary[i]
    }
    raise "[bug] prime not found greater than #{v}"
  end

  def make_basic_block(prime)
    prime.times {|i|
      prime.times {|j|
        row = [i]
        0.upto(prime-1) {|m|
          row << (i*m + j) % prime
        }
        yield row
      }
    }
  end

  def combine_block(tbl1, tbl2)
    result = []
    tbl2.each {|row|
      result << row * tbl1.first.length
    }
    tbl1.each_with_index {|row, k|
      next if k == 0
      result << row.map {|i| [i] * tbl2.first.length }.flatten
    }
    result
  end

  def make_large_block(v, prime)
    if prime <= v+1
      make_basic_block(v) {|row|
        yield row
      }
    else
      tbl = []
      make_basic_block(v) {|row|
        tbl << row
      }
      tbls = [tbl]
      while tbl.first.length ** 2 < prime
        tbl = combine_block(tbl, tbl)
        tbls << tbl
      end
      tbl1 = tbls.find {|t| prime <= t.first.length * tbl.first.length }
      tbl = combine_block(tbl, tbl1)
      tbl.each {|row|
        yield row
      }
    end
  end

  def each_index(*vs)
    n = vs.length
    max_v = vs.max
    prime = make_prime(max_v)
    h = {}
    make_large_block(max_v, n) {|row|
      row = vs.zip(row).map {|v, i| i % v }
      next if h[row]
      h[row] = true
      yield row
    }
  end

  # generate all pairs test.
  def each(*args)
    args.map! {|a| a.to_a }
    each_index(*args.map {|a| a.length}) {|is|
      yield is.zip(args).map {|i, a| a[i] }
    }
  end

  # generate all combination in cartesian product.  (not all-pairs test)
  def exhaustive_each(*args)
    args = args.map {|a| a.to_a }
    i = 0
    while true
      n = i
      as = []
      args.reverse_each {|a|
        n, m = n.divmod(a.length)
        as.unshift a[m]
      }
      break if 0 < n
      yield as
      i += 1
    end
  end
end
