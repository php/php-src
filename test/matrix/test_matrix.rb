require 'test/unit'
require 'matrix'

class TestMatrix < Test::Unit::TestCase
  def setup
    @m1 = Matrix[[1,2,3], [4,5,6]]
    @m2 = Matrix[[1,2,3], [4,5,6]]
    @m3 = @m1.clone
    @m4 = Matrix[[1.0, 2.0, 3.0], [4.0, 5.0, 6.0]]
    @n1 = Matrix[[2,3,4], [5,6,7]]
  end

  def test_matrix
    assert_equal(1, @m1[0, 0])
    assert_equal(2, @m1[0, 1])
    assert_equal(3, @m1[0, 2])
    assert_equal(4, @m1[1, 0])
    assert_equal(5, @m1[1, 1])
    assert_equal(6, @m1[1, 2])
  end

  def test_identity
    assert_same @m1, @m1
    assert_not_same @m1, @m2
    assert_not_same @m1, @m3
    assert_not_same @m1, @m4
    assert_not_same @m1, @n1
  end

  def test_equality
    assert_equal @m1, @m1
    assert_equal @m1, @m2
    assert_equal @m1, @m3
    assert_equal @m1, @m4
    assert_not_equal @m1, @n1
  end

  def test_hash_equality
    assert @m1.eql?(@m1)
    assert @m1.eql?(@m2)
    assert @m1.eql?(@m3)
    assert !@m1.eql?(@m4)
    assert !@m1.eql?(@n1)

    hash = { @m1 => :value }
    assert hash.key?(@m1)
    assert hash.key?(@m2)
    assert hash.key?(@m3)
    assert !hash.key?(@m4)
    assert !hash.key?(@n1)
  end

  def test_hash
    assert_equal @m1.hash, @m1.hash
    assert_equal @m1.hash, @m2.hash
    assert_equal @m1.hash, @m3.hash
  end

  def test_rank
    [
      [[0]],
      [[0], [0]],
      [[0, 0], [0, 0]],
      [[0, 0], [0, 0], [0, 0]],
      [[0, 0, 0]],
      [[0, 0, 0], [0, 0, 0]],
      [[0, 0, 0], [0, 0, 0], [0, 0, 0]],
      [[0, 0, 0], [0, 0, 0], [0, 0, 0], [0, 0, 0]],
    ].each do |rows|
      assert_equal 0, Matrix[*rows].rank
    end

    [
      [[1], [0]],
      [[1, 0], [0, 0]],
      [[1, 0], [1, 0]],
      [[0, 0], [1, 0]],
      [[1, 0], [0, 0], [0, 0]],
      [[0, 0], [1, 0], [0, 0]],
      [[0, 0], [0, 0], [1, 0]],
      [[1, 0], [1, 0], [0, 0]],
      [[0, 0], [1, 0], [1, 0]],
      [[1, 0], [1, 0], [1, 0]],
      [[1, 0, 0]],
      [[1, 0, 0], [0, 0, 0]],
      [[0, 0, 0], [1, 0, 0]],
      [[1, 0, 0], [1, 0, 0]],
      [[1, 0, 0], [1, 0, 0]],
      [[1, 0, 0], [0, 0, 0], [0, 0, 0]],
      [[0, 0, 0], [1, 0, 0], [0, 0, 0]],
      [[0, 0, 0], [0, 0, 0], [1, 0, 0]],
      [[1, 0, 0], [1, 0, 0], [0, 0, 0]],
      [[0, 0, 0], [1, 0, 0], [1, 0, 0]],
      [[1, 0, 0], [0, 0, 0], [1, 0, 0]],
      [[1, 0, 0], [1, 0, 0], [1, 0, 0]],
      [[1, 0, 0], [0, 0, 0], [0, 0, 0], [0, 0, 0]],
      [[1, 0, 0], [0, 0, 0], [0, 0, 0], [0, 0, 0]],
      [[1, 0, 0], [1, 0, 0], [0, 0, 0], [0, 0, 0]],
      [[1, 0, 0], [0, 0, 0], [1, 0, 0], [0, 0, 0]],
      [[1, 0, 0], [0, 0, 0], [0, 0, 0], [1, 0, 0]],
      [[1, 0, 0], [1, 0, 0], [1, 0, 0], [0, 0, 0]],
      [[1, 0, 0], [0, 0, 0], [1, 0, 0], [1, 0, 0]],
      [[1, 0, 0], [1, 0, 0], [0, 0, 0], [1, 0, 0]],
      [[1, 0, 0], [1, 0, 0], [1, 0, 0], [1, 0, 0]],

      [[1]],
      [[1], [1]],
      [[1, 1]],
      [[1, 1], [1, 1]],
      [[1, 1], [1, 1], [1, 1]],
      [[1, 1, 1]],
      [[1, 1, 1], [1, 1, 1]],
      [[1, 1, 1], [1, 1, 1], [1, 1, 1]],
      [[1, 1, 1], [1, 1, 1], [1, 1, 1], [1, 1, 1]],
    ].each do |rows|
      matrix = Matrix[*rows]
      assert_equal 1, matrix.rank
      assert_equal 1, matrix.transpose.rank
    end

    [
      [[1, 0], [0, 1]],
      [[1, 0], [0, 1], [0, 0]],
      [[1, 0], [0, 1], [0, 1]],
      [[1, 0], [0, 1], [1, 1]],
      [[1, 0, 0], [0, 1, 0]],
      [[1, 0, 0], [0, 0, 1]],
      [[1, 0, 0], [0, 1, 0], [0, 0, 0]],
      [[1, 0, 0], [0, 0, 1], [0, 0, 0]],

      [[1, 0, 0], [0, 0, 0], [0, 1, 0]],
      [[1, 0, 0], [0, 0, 0], [0, 0, 1]],

      [[1, 0], [1, 1]],
      [[1, 2], [1, 1]],
      [[1, 2], [0, 1], [1, 1]],
    ].each do |rows|
      m = Matrix[*rows]
      assert_equal 2, m.rank
      assert_equal 2, m.transpose.rank
    end

    [
      [[1, 0, 0], [0, 1, 0], [0, 0, 1]],
      [[1, 1, 0], [0, 1, 1], [1, 0, 1]],
      [[1, 1, 0], [0, 1, 1], [1, 0, 1]],
      [[1, 1, 0], [0, 1, 1], [1, 0, 1], [0, 0, 0]],
      [[1, 1, 0], [0, 1, 1], [1, 0, 1], [1, 1, 1]],
      [[1, 1, 1], [1, 1, 2], [1, 3, 1], [4, 1, 1]],
    ].each do |rows|
      m = Matrix[*rows]
      assert_equal 3, m.rank
      assert_equal 3, m.transpose.rank
    end
  end

  def test_inverse
    assert_equal(Matrix[[-1, 1], [0, -1]], Matrix[[-1, -1], [0, -1]].inverse)
  end

  def test_determinant
    assert_equal(45, Matrix[[7,6], [3,9]].determinant)
    assert_equal(-18, Matrix[[2,0,1],[0,-2,2],[1,2,3]].determinant)
  end

  def test_new_matrix
    assert_raise(TypeError) { Matrix[Object.new] }
    o = Object.new
    def o.to_ary; [1,2,3]; end
    assert_equal(@m1, Matrix[o, [4,5,6]])
  end

  def test_rows
    assert_equal(@m1, Matrix.rows([[1, 2, 3], [4, 5, 6]]))
  end

  def test_columns
    assert_equal(@m1, Matrix.columns([[1, 4], [2, 5], [3, 6]]))
  end

  def test_diagonal
    assert_equal(Matrix[[3,0,0],[0,2,0],[0,0,1]], Matrix.diagonal(3, 2, 1))
    assert_equal(Matrix[[4,0,0,0],[0,3,0,0],[0,0,2,0],[0,0,0,1]], Matrix.diagonal(4, 3, 2, 1))
  end

  def test_scalar
    assert_equal(Matrix[[2,0,0],[0,2,0],[0,0,2]], Matrix.scalar(3, 2))
    assert_equal(Matrix[[2,0,0,0],[0,2,0,0],[0,0,2,0],[0,0,0,2]], Matrix.scalar(4, 2))
  end

  def test_identity2
    assert_equal(Matrix[[1,0,0],[0,1,0],[0,0,1]], Matrix.identity(3))
    assert_equal(Matrix[[1,0,0],[0,1,0],[0,0,1]], Matrix.unit(3))
    assert_equal(Matrix[[1,0,0],[0,1,0],[0,0,1]], Matrix.I(3))
    assert_equal(Matrix[[1,0,0,0],[0,1,0,0],[0,0,1,0],[0,0,0,1]], Matrix.identity(4))
  end

  def test_zero
    assert_equal(Matrix[[0,0,0],[0,0,0],[0,0,0]], Matrix.zero(3))
    assert_equal(Matrix[[0,0,0,0],[0,0,0,0],[0,0,0,0],[0,0,0,0]], Matrix.zero(4))
    assert_equal(Matrix[[0]], Matrix.zero(1))
  end

  def test_row_vector
    assert_equal(Matrix[[1,2,3,4]], Matrix.row_vector([1,2,3,4]))
  end

  def test_column_vector
    assert_equal(Matrix[[1],[2],[3],[4]], Matrix.column_vector([1,2,3,4]))
  end

  def test_empty
    m = Matrix.empty(2, 0)
    assert_equal(Matrix[ [], [] ], m)
    n = Matrix.empty(0, 3)
    assert_equal(Matrix.columns([ [], [], [] ]), n)
    assert_equal(Matrix[[0, 0, 0], [0, 0, 0]], m * n)
  end

  def test_row
    assert_equal(Vector[1, 2, 3], @m1.row(0))
    assert_equal(Vector[4, 5, 6], @m1.row(1))
    a = []; @m1.row(0) {|x| a << x }
    assert_equal([1, 2, 3], a)
  end

  def test_column
    assert_equal(Vector[1, 4], @m1.column(0))
    assert_equal(Vector[2, 5], @m1.column(1))
    assert_equal(Vector[3, 6], @m1.column(2))
    a = []; @m1.column(0) {|x| a << x }
    assert_equal([1, 4], a)
  end

  def test_collect
    assert_equal(Matrix[[1, 4, 9], [16, 25, 36]], @m1.collect {|x| x ** 2 })
  end

  def test_minor
    assert_equal(Matrix[[1, 2], [4, 5]], @m1.minor(0..1, 0..1))
    assert_equal(Matrix[[2], [5]], @m1.minor(0..1, 1..1))
    assert_equal(Matrix[[4, 5]], @m1.minor(1..1, 0..1))
    assert_equal(Matrix[[1, 2], [4, 5]], @m1.minor(0, 2, 0, 2))
    assert_equal(Matrix[[4, 5]], @m1.minor(1, 1, 0, 2))
    assert_equal(Matrix[[2], [5]], @m1.minor(0, 2, 1, 1))
    assert_raise(ArgumentError) { @m1.minor(0) }
  end

  def test_regular?
    assert(Matrix[[1, 0], [0, 1]].regular?)
    assert(Matrix[[1, 0, 0], [0, 1, 0], [0, 0, 1]].regular?)
    assert(!Matrix[[1, 0, 0], [0, 0, 1], [0, 0, 1]].regular?)
  end

  def test_singular?
    assert(!Matrix[[1, 0], [0, 1]].singular?)
    assert(!Matrix[[1, 0, 0], [0, 1, 0], [0, 0, 1]].singular?)
    assert(Matrix[[1, 0, 0], [0, 0, 1], [0, 0, 1]].singular?)
  end

  def test_square?
    assert(Matrix[[1, 0], [0, 1]].square?)
    assert(Matrix[[1, 0, 0], [0, 1, 0], [0, 0, 1]].square?)
    assert(Matrix[[1, 0, 0], [0, 0, 1], [0, 0, 1]].square?)
    assert(!Matrix[[1, 0, 0], [0, 1, 0]].square?)
  end

  def test_mul
    assert_equal(Matrix[[2,4],[6,8]], Matrix[[2,4],[6,8]] * Matrix.I(2))
    assert_equal(Matrix[[4,8],[12,16]], Matrix[[2,4],[6,8]] * 2)
    assert_equal(Matrix[[4,8],[12,16]], 2 * Matrix[[2,4],[6,8]])
    assert_equal(Matrix[[14,32],[32,77]], @m1 * @m1.transpose)
    assert_equal(Matrix[[17,22,27],[22,29,36],[27,36,45]], @m1.transpose * @m1)
    assert_equal(Vector[14,32], @m1 * Vector[1,2,3])
    o = Object.new
    def o.coerce(m)
      [m, m.transpose]
    end
    assert_equal(Matrix[[14,32],[32,77]], @m1 * o)
  end

  def test_add
    assert_equal(Matrix[[6,0],[-4,12]], Matrix.scalar(2,5) + Matrix[[1,0],[-4,7]])
    assert_equal(Matrix[[3,5,7],[9,11,13]], @m1 + @n1)
    assert_equal(Matrix[[3,5,7],[9,11,13]], @n1 + @m1)
    assert_equal(Matrix[[2],[4],[6]], Matrix[[1],[2],[3]] + Vector[1,2,3])
    assert_raise(Matrix::ErrOperationNotDefined) { @m1 + 1 }
    o = Object.new
    def o.coerce(m)
      [m, m]
    end
    assert_equal(Matrix[[2,4,6],[8,10,12]], @m1 + o)
  end

  def test_sub
    assert_equal(Matrix[[4,0],[4,-2]], Matrix.scalar(2,5) - Matrix[[1,0],[-4,7]])
    assert_equal(Matrix[[-1,-1,-1],[-1,-1,-1]], @m1 - @n1)
    assert_equal(Matrix[[1,1,1],[1,1,1]], @n1 - @m1)
    assert_equal(Matrix[[0],[0],[0]], Matrix[[1],[2],[3]] - Vector[1,2,3])
    assert_raise(Matrix::ErrOperationNotDefined) { @m1 - 1 }
    o = Object.new
    def o.coerce(m)
      [m, m]
    end
    assert_equal(Matrix[[0,0,0],[0,0,0]], @m1 - o)
  end

  def test_div
    assert_equal(Matrix[[0,1,1],[2,2,3]], @m1 / 2)
    assert_equal(Matrix[[1,1],[1,1]], Matrix[[2,2],[2,2]] / Matrix.scalar(2,2))
    o = Object.new
    def o.coerce(m)
      [m, Matrix.scalar(2,2)]
    end
    assert_equal(Matrix[[1,1],[1,1]], Matrix[[2,2],[2,2]] / o)
  end

  def test_exp
    assert_equal(Matrix[[67,96],[48,99]], Matrix[[7,6],[3,9]] ** 2)
    assert_equal(Matrix.I(5), Matrix.I(5) ** -1)
    assert_raise(Matrix::ErrOperationNotDefined) { Matrix.I(5) ** Object.new }
  end

  def test_det
    assert_equal(45, Matrix[[7,6],[3,9]].det)
    assert_equal(0, Matrix[[0,0],[0,0]].det)
    assert_equal(-7, Matrix[[0,0,1],[0,7,6],[1,3,9]].det)
    assert_equal(42, Matrix[[7,0,1,0,12],[8,1,1,9,1],[4,0,0,-7,17],[-1,0,0,-4,8],[10,1,1,8,6]].det)
  end

  def test_rank2
    assert_equal(2, Matrix[[7,6],[3,9]].rank)
    assert_equal(0, Matrix[[0,0],[0,0]].rank)
    assert_equal(3, Matrix[[0,0,1],[0,7,6],[1,3,9]].rank)
    assert_equal(1, Matrix[[0,1],[0,1],[0,1]].rank)
    assert_equal(2, @m1.rank)
  end

  def test_trace
    assert_equal(1+5+9, Matrix[[1,2,3],[4,5,6],[7,8,9]].trace)
  end

  def test_transpose
    assert_equal(Matrix[[1,4],[2,5],[3,6]], @m1.transpose)
  end

  def test_row_vectors
    assert_equal([Vector[1,2,3], Vector[4,5,6]], @m1.row_vectors)
  end

  def test_column_vectors
    assert_equal([Vector[1,4], Vector[2,5], Vector[3,6]], @m1.column_vectors)
  end

  def test_to_s
    assert_equal("Matrix[[1, 2, 3], [4, 5, 6]]", @m1.to_s)
    assert_equal("Matrix.empty(0, 0)", Matrix[].to_s)
    assert_equal("Matrix.empty(1, 0)", Matrix[[]].to_s)
  end

  def test_inspect
    assert_equal("Matrix[[1, 2, 3], [4, 5, 6]]", @m1.inspect)
    assert_equal("Matrix.empty(0, 0)", Matrix[].inspect)
    assert_equal("Matrix.empty(1, 0)", Matrix[[]].inspect)
  end

  def test_scalar_add
    s1 = @m1.coerce(1).first
    assert_equal(Matrix[[1]], (s1 + 0) * Matrix[[1]])
    assert_raise(Matrix::ErrOperationNotDefined) { s1 + Vector[0] }
    assert_raise(Matrix::ErrOperationNotDefined) { s1 + Matrix[[0]] }
    o = Object.new
    def o.coerce(x)
      [1, 1]
    end
    assert_equal(2, s1 + o)
  end

  def test_scalar_sub
    s1 = @m1.coerce(1).first
    assert_equal(Matrix[[1]], (s1 - 0) * Matrix[[1]])
    assert_raise(Matrix::ErrOperationNotDefined) { s1 - Vector[0] }
    assert_raise(Matrix::ErrOperationNotDefined) { s1 - Matrix[[0]] }
    o = Object.new
    def o.coerce(x)
      [1, 1]
    end
    assert_equal(0, s1 - o)
  end

  def test_scalar_mul
    s1 = @m1.coerce(1).first
    assert_equal(Matrix[[1]], (s1 * 1) * Matrix[[1]])
    assert_equal(Vector[2], s1 * Vector[2])
    assert_equal(Matrix[[2]], s1 * Matrix[[2]])
    o = Object.new
    def o.coerce(x)
      [1, 1]
    end
    assert_equal(1, s1 * o)
  end

  def test_scalar_div
    s1 = @m1.coerce(1).first
    assert_equal(Matrix[[1]], (s1 / 1) * Matrix[[1]])
    assert_raise(Matrix::ErrOperationNotDefined) { s1 / Vector[0] }
    assert_equal(Matrix[[Rational(1,2)]], s1 / Matrix[[2]])
    o = Object.new
    def o.coerce(x)
      [1, 1]
    end
    assert_equal(1, s1 / o)
  end

  def test_scalar_pow
    s1 = @m1.coerce(1).first
    assert_equal(Matrix[[1]], (s1 ** 1) * Matrix[[1]])
    assert_raise(Matrix::ErrOperationNotDefined) { s1 ** Vector[0] }
    assert_raise(Matrix::ErrOperationNotImplemented) { s1 ** Matrix[[1]] }
    o = Object.new
    def o.coerce(x)
      [1, 1]
    end
    assert_equal(1, s1 ** o)
  end
end
