--TEST--
preg_split() with CompiledRegex
--FILE--
<?php

$string = 'this is a_list: value1, Test__, string; Hello, world!_(parentheses)';

// parts of $string separated by : , ; ( or ) are put into an array.
$o = new Regex\CompiledRegex('[:,;\(\)]');
var_dump(preg_split($o, $string, -1, PREG_SPLIT_NO_EMPTY));

// all text between : and ; is removed
$o = new Regex\CompiledRegex(':\s*(\w*,*\s*)+;');
var_dump(preg_split($o, $string));

// all text before (parentheses) is put into first element, ( into second, "parentheses" into third and ) into fourth.
$o = new Regex\CompiledRegex('(\(|\))');
var_dump(preg_split($o, $string, -1, PREG_SPLIT_DELIM_CAPTURE|PREG_SPLIT_NO_EMPTY));

// tries to find NAME regardless of case in $string (can't split it so just returns how string as first element)
$o = new Regex\CompiledRegex('NAME', caseSensitive: false);
var_dump(preg_split($o, $string));

//every character (including whitespace) is put into an array element
$o = new Regex\CompiledRegex('\w');
var_dump(preg_split($o, $string, -1, PREG_SPLIT_NO_EMPTY));

?>
--EXPECT--
array(7) {
  [0]=>
  string(14) "this is a_list"
  [1]=>
  string(7) " value1"
  [2]=>
  string(7) " Test__"
  [3]=>
  string(7) " string"
  [4]=>
  string(6) " Hello"
  [5]=>
  string(8) " world!_"
  [6]=>
  string(11) "parentheses"
}
array(2) {
  [0]=>
  string(14) "this is a_list"
  [1]=>
  string(28) " Hello, world!_(parentheses)"
}
array(4) {
  [0]=>
  string(54) "this is a_list: value1, Test__, string; Hello, world!_"
  [1]=>
  string(1) "("
  [2]=>
  string(11) "parentheses"
  [3]=>
  string(1) ")"
}
array(1) {
  [0]=>
  string(67) "this is a_list: value1, Test__, string; Hello, world!_(parentheses)"
}
array(10) {
  [0]=>
  string(1) " "
  [1]=>
  string(1) " "
  [2]=>
  string(2) ": "
  [3]=>
  string(2) ", "
  [4]=>
  string(2) ", "
  [5]=>
  string(2) "; "
  [6]=>
  string(2) ", "
  [7]=>
  string(1) "!"
  [8]=>
  string(1) "("
  [9]=>
  string(1) ")"
}
