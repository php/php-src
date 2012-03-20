# -*- coding: UTF-8 -*-
require 'test/unit'
require 'erb'

class TestERB < Test::Unit::TestCase
  def test_result_encoding
    erb = ERB.new("hello")
    assert_equal __ENCODING__, erb.result.encoding

    erb = ERB.new("こんにちは".encode("EUC-JP"))
    assert_equal Encoding::EUC_JP, erb.result.encoding

    erb = ERB.new("\xC4\xE3\xBA\xC3".force_encoding("EUC-CN"))
    assert_equal Encoding::EUC_CN, erb.result.encoding

    erb = ERB.new("γεια σας".encode("ISO-8859-7"))
    assert_equal Encoding::ISO_8859_7, erb.result.encoding

    assert_raise(ArgumentError, /ASCII compatible/) {
      ERB.new("こんにちは".force_encoding("ISO-2022-JP")) # dummy encoding
    }
  end

  def test_generate_magic_comment
    erb = ERB.new("hello")
    assert_match(/#coding:UTF-8/, erb.src)

    erb = ERB.new("hello".force_encoding("EUC-JP"))
    assert_match(/#coding:EUC-JP/, erb.src)

    erb = ERB.new("hello".force_encoding("ISO-8859-9"))
    assert_match(/#coding:ISO-8859-9/, erb.src)
  end

  def test_literal_encoding
    erb = ERB.new("literal encoding is <%= 'hello'.encoding  %>")
    assert_match(/literal encoding is UTF-8/, erb.result)

    erb = ERB.new("literal encoding is <%= 'こんにちは'.encoding  %>".encode("EUC-JP"))
    assert_match(/literal encoding is EUC-JP/, erb.result)

    erb = ERB.new("literal encoding is <%= '\xC4\xE3\xBA\xC3'.encoding %>".force_encoding("EUC-CN"))
    assert_match(/literal encoding is GB2312/, erb.result)
  end

  def test___ENCODING__
    erb = ERB.new("__ENCODING__ is <%= __ENCODING__ %>")
    assert_match(/__ENCODING__ is UTF-8/, erb.result)

    erb = ERB.new("__ENCODING__ is <%= __ENCODING__ %>".force_encoding("EUC-JP"))
    assert_match(/__ENCODING__ is EUC-JP/, erb.result)

    erb = ERB.new("__ENCODING__ is <%= __ENCODING__ %>".force_encoding("Big5"))
    assert_match(/__ENCODING__ is Big5/, erb.result)
  end

  def test_recognize_magic_comment
    erb = ERB.new(<<-EOS.encode("EUC-KR"))
<%# -*- coding: EUC-KR -*- %>
안녕하세요
    EOS
    assert_match(/#coding:EUC-KR/, erb.src)
    assert_equal Encoding::EUC_KR, erb.result.encoding

    erb = ERB.new(<<-EOS.encode("EUC-KR").force_encoding("ASCII-8BIT"))
<%#-*- coding: EUC-KR -*-%>
안녕하세요
    EOS
    assert_match(/#coding:EUC-KR/, erb.src)
    assert_equal Encoding::EUC_KR, erb.result.encoding

    erb = ERB.new(<<-EOS.encode("EUC-KR").force_encoding("ASCII-8BIT"))
<%# vim: tabsize=8 encoding=EUC-KR shiftwidth=2 expandtab %>
안녕하세요
    EOS
    assert_match(/#coding:EUC-KR/, erb.src)
    assert_equal Encoding::EUC_KR, erb.result.encoding

    erb = ERB.new(<<-EOS.encode("EUC-KR").force_encoding("ASCII-8BIT"))
<%#coding:EUC-KR %>
안녕하세요
    EOS
    assert_match(/#coding:EUC-KR/, erb.src)
    assert_equal Encoding::EUC_KR, erb.result.encoding

    erb = ERB.new(<<-EOS.encode("EUC-KR").force_encoding("EUC-JP"))
<%#coding:EUC-KR %>
안녕하세요
    EOS
    assert_match(/#coding:EUC-KR/, erb.src)
    assert_equal Encoding::EUC_KR, erb.result.encoding
  end

  module M; end
  def test_method_with_encoding
    obj = Object.new
    obj.extend(M)

    erb = ERB.new(<<-EOS.encode("EUC-JP").force_encoding("ASCII-8BIT"))
<%#coding:EUC-JP %>
literal encoding is <%= 'こんにちは'.encoding %>
__ENCODING__ is <%= __ENCODING__ %>
    EOS
    erb.def_method(M, :m_from_magic_comment)

    result = obj.m_from_magic_comment
    assert_equal Encoding::EUC_JP, result.encoding
    assert_match(/literal encoding is EUC-JP/, result)
    assert_match(/__ENCODING__ is EUC-JP/, result)

    erb = ERB.new(<<-EOS.encode("EUC-KR"))
literal encoding is <%= '안녕하세요'.encoding %>
__ENCODING__ is <%= __ENCODING__ %>
EOS
    erb.def_method(M, :m_from_eval_encoding)
    result = obj.m_from_eval_encoding
    assert_equal Encoding::EUC_KR, result.encoding
    assert_match(/literal encoding is EUC-KR/, result)
    assert_match(/__ENCODING__ is EUC-KR/, result)
  end
end

# vim:fileencoding=UTF-8
