--TEST--
parse_ini_file() tests
--ENV--
basicval=FUBAR_VARIABLE
basicqval=FUBAR_QUOTES_VARIABLE
--FILE--
<?php

$ini_file = dirname(__FILE__)."/parse_ini_basic.data";

define('TEST_CONSTANT', 'this_is_test_constant');

var_dump(parse_ini_file($ini_file, 1));

echo "Done.\n";
?>
--EXPECTF--	
array(26) {
  ["basic"]=>
  array(15) {
    ["basicval"]=>
    string(3) "bar"
    ["longval"]=>
    string(5) "12345"
    ["with.dot"]=>
    string(7) "fooobar"
    ["boolon"]=>
    string(1) "1"
    ["booltrue"]=>
    string(1) "1"
    ["boolyes"]=>
    string(1) "1"
    ["booloff"]=>
    string(0) ""
    ["boolfalse"]=>
    string(0) ""
    ["boolnone"]=>
    string(0) ""
    ["boolno"]=>
    string(0) ""
    ["string"]=>
    string(34) "asdadfsdjkslkj ¡@£$$ { }[ ]/%#¤"
    ["sqstring"]=>
    string(14) "adsasdadasdasd"
    ["dqstring"]=>
    string(51) "asdadfsdjkslkj ¡@£$$ { } !^~|¥¥{[()/)&/% ¤ # #"
    ["php_constant"]=>
    string(1) "2"
    ["user_constant"]=>
    string(21) "this_is_test_constant"
  }
  ["basic with whitespace"]=>
  array(13) {
    ["basicval"]=>
    string(3) "bar"
    ["longval"]=>
    string(5) "12345"
    ["with.dot"]=>
    string(7) "fooobar"
    ["boolon"]=>
    string(1) "1"
    ["booltrue"]=>
    string(1) "1"
    ["boolyes"]=>
    string(1) "1"
    ["booloff"]=>
    string(0) ""
    ["boolfalse"]=>
    string(0) ""
    ["boolnone"]=>
    string(0) ""
    ["boolno"]=>
    string(0) ""
    ["sqstring"]=>
    string(14) "adsasdadasdasd"
    ["dqstring"]=>
    string(41) "asdadfsdjkslkj ¡@£$$€¥¥{[()/)&/%#¤"
    ["php_constant"]=>
    string(1) "2"
  }
  ["comments"]=>
  array(1) {
    ["somecomment"]=>
    string(15) "comment follows"
  }
  ["variables"]=>
  array(8) {
    ["var1"]=>
    string(14) "FUBAR_VARIABLE"
    ["var2"]=>
    string(18) "FUBAR_VARIABLE/foo"
    ["var3"]=>
    string(18) "foo/FUBAR_VARIABLE"
    ["var4"]=>
    string(22) "foo/FUBAR_VARIABLE/foo"
    ["quoted_var1"]=>
    string(21) "FUBAR_QUOTES_VARIABLE"
    ["quoted_var2"]=>
    string(25) "FUBAR_QUOTES_VARIABLE/foo"
    ["quoted_var3"]=>
    string(25) "foo/FUBAR_QUOTES_VARIABLE"
    ["quoted_var4"]=>
    string(29) "foo/FUBAR_QUOTES_VARIABLE/foo"
  }
  ["offset values"]=>
  array(6) {
    ["foo1"]=>
    array(2) {
      [0]=>
      string(14) "basic offset 1"
      [1]=>
      string(14) "basic offset 2"
    }
    ["foo2"]=>
    array(1) {
      [123]=>
      string(11) "long offset"
    }
    ["foo3"]=>
    array(1) {
      ["abc"]=>
      string(13) "string offset"
    }
    ["foo4"]=>
    array(4) {
      [0]=>
      string(15) "quoted offset 1"
      [" "]=>
      string(15) "quoted offset 2"
      ["sqfoobar"]=>
      string(20) "quoted string offset"
      ["dqfoobar"]=>
      string(20) "single quoted offset"
    }
    ["foo6"]=>
    array(4) {
      ["FUBAR_VARIABLE"]=>
      string(8) "variable"
      ["FUBAR_VARIABLE/foo"]=>
      string(22) "variable with string 1"
      ["foo/FUBAR_VARIABLE"]=>
      string(22) "variable with string 2"
      ["foo/FUBAR_VARIABLE/foo"]=>
      string(22) "variable with string 3"
    }
    ["foo7"]=>
    array(4) {
      ["FUBAR_QUOTES_VARIABLE"]=>
      string(17) "quoted variable 1"
      ["FUBAR_QUOTES_VARIABLE/foo"]=>
      string(17) "quoted variable 2"
      ["foo/FUBAR_QUOTES_VARIABLE"]=>
      string(17) "quoted variable 3"
      ["foo/FUBAR_QUOTES_VARIABLE/foo"]=>
      string(17) "quoted variable 4"
    }
  }
  ["non value"]=>
  array(4) {
    ["novalue_option1"]=>
    string(0) ""
    ["novalue_option2"]=>
    string(0) ""
    ["novalue_option3"]=>
    string(0) ""
    ["novalue_option4"]=>
    array(3) {
      [0]=>
      string(0) ""
      [1]=>
      string(0) ""
      [2]=>
      string(0) ""
    }
  }
  ["Quoted strings and variables in sections"]=>
  array(0) {
  }
  ["FUBAR_VARIABLE"]=>
  array(0) {
  }
  ["FUBAR_VARIABLE/foo"]=>
  array(0) {
  }
  ["foo/FUBAR_VARIABLE"]=>
  array(0) {
  }
  ["foo/FUBAR_VARIABLE/foo"]=>
  array(0) {
  }
  ["FUBAR_QUOTES_VARIABLE"]=>
  array(0) {
  }
  ["FUBAR_QUOTES_VARIABLE/foo"]=>
  array(0) {
  }
  ["foo/FUBAR_QUOTES_VARIABLE"]=>
  array(0) {
  }
  ["foo/FUBAR_QUOTES_VARIABLE/foo"]=>
  array(0) {
  }
  ["PATH=FUBAR_VARIABLE/no/quotes"]=>
  array(0) {
  }
  ["PATH=FUBAR_VARIABLE/all/quoted"]=>
  array(0) {
  }
  ["01"]=>
  array(2) {
    ["e"]=>
    string(1) "e"
    ["f"]=>
    string(1) "f"
  }
  ["02"]=>
  array(2) {
    ["g"]=>
    string(1) "g"
    ["h"]=>
    string(1) "h"
  }
  [1]=>
  array(2) {
    ["a"]=>
    string(1) "a"
    ["b"]=>
    string(1) "b"
  }
  [2]=>
  array(2) {
    ["c"]=>
    string(1) "c"
    ["d"]=>
    string(1) "d"
  }
  ["0815"]=>
  array(1) {
    ["bla"]=>
    string(3) "bla"
  }
  ["bug #43923"]=>
  array(3) {
    ["curly1"]=>
    string(1) "{"
    ["curly2"]=>
    string(1) "{"
    ["curly3"]=>
    string(1) "{"
  }
  ["bug #44019"]=>
  array(6) {
    ["concatenation_before"]=>
    string(37) "this_is_test_constant+some_text_after"
    ["concatenation_middle"]=>
    string(54) "some_text_before+this_is_test_constant+some_text_after"
    ["concatenation_after"]=>
    string(38) "some_text_before+this_is_test_constant"
    ["concatenation_nows_before"]=>
    string(37) "this_is_test_constant+some_text_after"
    ["concatenation_nows_middle"]=>
    string(54) "some_text_before+this_is_test_constant+some_text_after"
    ["concatenation_nows_after"]=>
    string(38) "some_text_before+this_is_test_constant"
  }
  ["bug #43915"]=>
  array(3) {
    ["ini_with-hyphen"]=>
    string(26) "with hyphen and underscore"
    ["ini.with-hyphen"]=>
    string(14) "dot and hyphen"
    ["ini-with.hyphen"]=>
    string(14) "hyphen and dot"
  }
  ["windows paths"]=>
  array(2) {
    ["winpath1"]=>
    string(69) "c:\some windows\path\test\new\r\quote " here\single ' quote\some more"
    ["winpath2"]=>
    string(13) "special case\"
  }
}
Done.
