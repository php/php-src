require 'test/unit'

module TestEOF
  def test_eof_0
    open_file("") {|f|
      assert_equal("", f.read(0))
      assert_equal("", f.read(0))
      assert_equal("", f.read)
      assert_equal("", f.read(0))
      assert_equal("", f.read(0))
    }
    open_file("") {|f|
      assert_nil(f.read(1))
      assert_equal("", f.read)
      assert_nil(f.read(1))
    }
    open_file("") {|f|
      s = "x"
      assert_equal("", f.read(nil, s))
      assert_equal("", s)
    }
    open_file("") {|f|
      s = "x"
      assert_nil(f.read(10, s))
      assert_equal("", s)
    }
  end

  def test_eof_0_rw
    return unless respond_to? :open_file_rw
    open_file_rw("") {|f|
      assert_equal("", f.read)
      assert_equal("", f.read)
      assert_equal(0, f.syswrite(""))
      assert_equal("", f.read)
    }
  end

  def test_eof_1
    open_file("a") {|f|
      assert_equal("", f.read(0))
      assert_equal("a", f.read(1))
      assert_equal("" , f.read(0))
      assert_equal("" , f.read(0))
      assert_equal("", f.read)
      assert_equal("", f.read(0))
      assert_equal("", f.read(0))
    }
    open_file("a") {|f|
      assert_equal("a", f.read(1))
      assert_nil(f.read(1))
    }
    open_file("a") {|f|
      assert_equal("a", f.read(2))
      assert_nil(f.read(1))
      assert_equal("", f.read)
      assert_nil(f.read(1))
    }
    open_file("a") {|f|
      assert_equal("a", f.read)
      assert_nil(f.read(1))
      assert_equal("", f.read)
      assert_nil(f.read(1))
    }
    open_file("a") {|f|
      assert_equal("a", f.read(2))
      assert_equal("", f.read)
      assert_equal("", f.read)
    }
    open_file("a") {|f|
      assert_equal("a", f.read)
      assert_equal("", f.read(0))
    }
    open_file("a") {|f|
      s = "x"
      assert_equal("a", f.read(nil, s))
      assert_equal("a", s)
    }
    open_file("a") {|f|
      s = "x"
      assert_equal("a", f.read(10, s))
      assert_equal("a", s)
    }
  end

  def test_eof_2
    open_file("") {|f|
      assert_equal("", f.read)
      assert(f.eof?)
    }
  end

  def test_eof_3
    open_file("") {|f|
      assert(f.eof?)
    }
  end

  module Seek
    def open_file_seek(content, pos)
      open_file(content) do |f|
        f.seek(pos)
        yield f
      end
    end

    def test_eof_0_seek
      open_file_seek("", 10) {|f|
        assert_equal(10, f.pos)
        assert_equal("", f.read(0))
        assert_equal("", f.read)
        assert_equal("", f.read(0))
        assert_equal("", f.read)
      }
    end

    def test_eof_1_seek
      open_file_seek("a", 10) {|f|
        assert_equal("", f.read)
        assert_equal("", f.read)
      }
      open_file_seek("a", 1) {|f|
        assert_equal("", f.read)
        assert_equal("", f.read)
      }
    end
  end
end
