--TEST--
Test eregi() function : basic functionality (with $regs)
--FILE--
<?php
/* Prototype  : proto int eregi(string pattern, string string [, array registers])
 * Description: Regular expression match 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

/*
 * Test a number of simple, valid matches with eregi, specifying $regs
 */

echo "*** Testing eregi() : basic functionality ***\n";

include(dirname(__FILE__) . '/regular_expressions.inc');

foreach ($expressions as $re) {
	list($pattern,$string) = $re;
	echo "--> Pattern: '$pattern'; string: '$string'\n";
	var_dump(eregi($pattern, $string, $regs));
	var_dump($regs);
}

echo "Done";
?>
--EXPECTF--
*** Testing eregi() : basic functionality ***
--> Pattern: '..(a|b|c)(a|b|c)..'; string: '--- ab ---'
int(6)
array(3) {
  [0]=>
  string(6) "- ab -"
  [1]=>
  string(1) "a"
  [2]=>
  string(1) "b"
}
--> Pattern: '()'; string: ''
int(1)
array(2) {
  [0]=>
  bool(false)
  [1]=>
  bool(false)
}
--> Pattern: '()'; string: 'abcdef'
int(1)
array(2) {
  [0]=>
  bool(false)
  [1]=>
  bool(false)
}
--> Pattern: '[x]|[^x]'; string: 'abcdef'
int(1)
array(1) {
  [0]=>
  string(1) "a"
}
--> Pattern: '(a{1})(a{1,}) (b{1,3}) (c+) (d?ddd|e)'; string: '--- aaa bbb ccc ddd ---'
int(15)
array(6) {
  [0]=>
  string(15) "aaa bbb ccc ddd"
  [1]=>
  string(1) "a"
  [2]=>
  string(2) "aa"
  [3]=>
  string(3) "bbb"
  [4]=>
  string(3) "ccc"
  [5]=>
  string(3) "ddd"
}
--> Pattern: '\\\`\^\.\[\$\(\)\|\*\+\?\{\''; string: '\`^.[$()|*+?{''
int(14)
array(1) {
  [0]=>
  string(14) "\`^.[$()|*+?{'"
}
--> Pattern: '\a'; string: 'a'
int(1)
array(1) {
  [0]=>
  string(1) "a"
}
--> Pattern: '[0-9][^0-9]'; string: '2a'
int(2)
array(1) {
  [0]=>
  string(2) "2a"
}
--> Pattern: '^[[:alnum:]]{62,62}$'; string: '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ'
int(62)
array(1) {
  [0]=>
  string(62) "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
}
--> Pattern: '^[[:digit:]]{5}'; string: '0123456789'
int(5)
array(1) {
  [0]=>
  string(5) "01234"
}
--> Pattern: '[[:digit:]]{5}$'; string: '0123456789'
int(5)
array(1) {
  [0]=>
  string(5) "56789"
}
--> Pattern: '[[:blank:]]{1,10}'; string: '
 	'
int(2)
array(1) {
  [0]=>
  string(2) " 	"
}
--> Pattern: '[[:print:]]{3}'; string: ' a '
int(3)
array(1) {
  [0]=>
  string(3) " a "
}
Done