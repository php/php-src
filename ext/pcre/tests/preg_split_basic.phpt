--TEST--
Test preg_split() function : basic functionality 
--INI--
--FILE--
<?php
/*
* proto array preg_split(string pattern, string subject [, int limit [, int flags]])
* Function is implemented in ext/pcre/php_pcre.c
*/
$string = 'this is a_list: value1, Test__, string; Hello, world!_(parentheses)';
var_dump(preg_split('/[:,;\(\)]/', $string, -1, PREG_SPLIT_NO_EMPTY)); //parts of $string seperated by : , ; ( or ) are put into an array.
var_dump(preg_split('/:\s*(\w*,*\s*)+;/', $string)); //all text between : and ; is removed
var_dump(preg_split('/(\(|\))/', $string, -1, PREG_SPLIT_DELIM_CAPTURE|PREG_SPLIT_NO_EMPTY)); //all text before (parentheses) is put into first element, ( into second, "parentheses" into third and ) into fourth.
var_dump(preg_split('/NAME/i', $string)); //tries to find NAME regardless of case in $string (can't split it so just returns how string as first element)
var_dump(preg_split('/\w/', $string, -1, PREG_SPLIT_NO_EMPTY)); //every character (including whitespace) is put into an array element

?>
--EXPECT--
array(7) {
  [0]=>
  unicode(14) "this is a_list"
  [1]=>
  unicode(7) " value1"
  [2]=>
  unicode(7) " Test__"
  [3]=>
  unicode(7) " string"
  [4]=>
  unicode(6) " Hello"
  [5]=>
  unicode(8) " world!_"
  [6]=>
  unicode(11) "parentheses"
}
array(2) {
  [0]=>
  unicode(14) "this is a_list"
  [1]=>
  unicode(28) " Hello, world!_(parentheses)"
}
array(4) {
  [0]=>
  unicode(54) "this is a_list: value1, Test__, string; Hello, world!_"
  [1]=>
  unicode(1) "("
  [2]=>
  unicode(11) "parentheses"
  [3]=>
  unicode(1) ")"
}
array(1) {
  [0]=>
  unicode(67) "this is a_list: value1, Test__, string; Hello, world!_(parentheses)"
}
array(10) {
  [0]=>
  unicode(1) " "
  [1]=>
  unicode(1) " "
  [2]=>
  unicode(2) ": "
  [3]=>
  unicode(2) ", "
  [4]=>
  unicode(2) ", "
  [5]=>
  unicode(2) "; "
  [6]=>
  unicode(2) ", "
  [7]=>
  unicode(1) "!"
  [8]=>
  unicode(1) "("
  [9]=>
  unicode(1) ")"
}
