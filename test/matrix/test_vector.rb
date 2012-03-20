require 'test/unit'
require 'matrix'

class TestVector < Test::Unit::TestCase
  def setup
    @v1 = Vector[1,2,3]
    @v2 = Vector[1,2,3]
    @v3 = @v1.clone
    @v4 = Vector[1.0, 2.0, 3.0]
    @w1 = Vector[2,3,4]
  end

  def test_identity
    assert_same @v1, @v1
    assert_not_same @v1, @v2
    assert_not_same @v1, @v3
    assert_not_same @v1, @v4
    assert_not_same @v1, @w1
  end

  def test_equality
    assert_equal @v1, @v1
    assert_equal @v1, @v2
    assert_equal @v1, @v3
    assert_equal @v1, @v4
    assert_not_equal @v1, @w1
  end

  def test_hash_equality
    assert @v1.eql?(@v1)
    assert @v1.eql?(@v2)
    assert @v1.eql?(@v3)
    assert !@v1.eql?(@v4)
    assert !@v1.eql?(@w1)

    hash = { @v1 => :value }
    assert hash.key?(@v1)
    assert hash.key?(@v2)
    assert hash.key?(@v3)
    assert !hash.key?(@v4)
    assert !hash.key?(@w1)
  end

  def test_hash
    assert_equal @v1.hash, @v1.hash
    assert_equal @v1.hash, @v2.hash
    assert_equal @v1.hash, @v3.hash
  end

  def test_aref
    assert_equal(1, @v1[0])
    assert_equal(2, @v1[1])
    assert_equal(3, @v1[2])
    assert_equal(3, @v1[-1])
    assert_equal(nil, @v1[3])
  end

  def test_size
    assert_equal(3, @v1.size)
  end

  def test_each2
    a = []
    @v1.each2(@v4) {|x, y| a << [x, y] }
    assert_equal([[1,1.0],[2,2.0],[3,3.0]], a)
  end

  def test_collect
    a = @v1.collect {|x| x + 1 }
    assert_equal(Vector[2,3,4], a)
  end

  def test_collect2
    a = @v1.collect2(@v4) {|x, y| x + y }
    assert_equal([2.0,4.0,6.0], a)
  end

  def test_map2
    a = @v1.map2(@v4) {|x, y| x + y }
    assert_equal(Vector[2.0,4.0,6.0], a)
  end

  def test_mul
    assert_equal(Vector[2,4,6], @v1 * 2)
    assert_equal(Matrix[[1, 4, 9], [2, 8, 18], [3, 12, 27]], @v1 * Matrix[[1,4,9]])
    assert_raise(Matrix::ErrOperationNotDefined) { @v1 * Vector[1,4,9] }
    o = Object.new
    def o.coerce(x)
      [1, 1]
    end
    assert_equal(1, Vector[1, 2, 3] * o)
  end

  def test_add
    assert_equal(Vector[2,4,6], @v1 + @v1)
    assert_equal(Matrix[[2],[6],[12]], @v1 + Matrix[[1],[4],[9]])
    o = Object.new
    def o.coerce(x)
      [1, 1]
    end
    assert_equal(2, Vector[1, 2, 3] + o)
  end

  def test_sub
    assert_equal(Vector[0,0,0], @v1 - @v1)
    assert_equal(Matrix[[0],[-2],[-6]], @v1 - Matrix[[1],[4],[9]])
    o = Object.new
    def o.coerce(x)
      [1, 1]
    end
    assert_equal(0, Vector[1, 2, 3] - o)
  end

  def test_inner_product
    assert_equal(1+4+9, @v1.inner_product(@v1))
  end

  def test_r
    assert_equal(5, Vector[3, 4].r)
  end

  def test_covector
    assert_equal(Matrix[[1,2,3]], @v1.covector)
  end

  def test_to_s
    assert_equal("Vector[1, 2, 3]", @v1.to_s)
  end

  def test_inspect
    assert_equal("Vector[1, 2, 3]", @v1.inspect)
  end

end
