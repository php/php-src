require_relative 'test_base'

require 'dl/cparser'

module DL
  class TestCParser < TestBase
    include DL::CParser

    def test_uint_ctype
      assert_equal(-DL::TYPE_INT, parse_ctype('uint'))
    end

    def test_size_t_ctype
      assert_equal(DL::TYPE_SIZE_T, parse_ctype("size_t"))
    end

    def test_ssize_t_ctype
      assert_equal(DL::TYPE_SSIZE_T, parse_ctype("ssize_t"))
    end

    def test_ptrdiff_t_ctype
      assert_equal(DL::TYPE_PTRDIFF_T, parse_ctype("ptrdiff_t"))
    end

    def test_intptr_t_ctype
      assert_equal(DL::TYPE_INTPTR_T, parse_ctype("intptr_t"))
    end

    def test_uintptr_t_ctype
      assert_equal(DL::TYPE_UINTPTR_T, parse_ctype("uintptr_t"))
    end
  end
end
