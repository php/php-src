--TEST--
Test split() function : basic functionality - test a number of simple split, specifying a limit
--FILE--
<?php
/* Prototype  : proto array split(string pattern, string string [, int limit])
 * Description: Split string into array by regular expression 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

/*
 * Test a number of simple split, specifying a limit
 */

echo "*** Testing ereg() : basic functionality ***\n";

include(dirname(__FILE__) . '/regular_expressions.inc');

foreach ($expressions as $re) {
	list($pattern,$string) = $re;
	echo "\n--> Pattern: '$pattern'; match: '$string'\n";
	var_dump(split($pattern, $string . ' |1| ' . $string . ' |2| ' . $string, 2));
}

echo "Done";
?>
--EXPECTF--
*** Testing ereg() : basic functionality ***

--> Pattern: '..(a|b|c)(a|b|c)..'; match: '--- ab ---'
array(2) {
  [0]=>
  string(2) "--"
  [1]=>
  string(32) "-- |1| --- ab --- |2| --- ab ---"
}

--> Pattern: '()'; match: ''

Warning: split(): Invalid Regular Expression in %s on line 19
bool(false)

--> Pattern: '()'; match: 'abcdef'

Warning: split(): Invalid Regular Expression in %s on line 19
bool(false)

--> Pattern: '[x]|[^x]'; match: 'abcdef'
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(27) "bcdef |1| abcdef |2| abcdef"
}

--> Pattern: '(a{1})(a{1,}) (b{1,3}) (c+) (d?ddd|e)'; match: '--- aaa bbb ccc ddd ---'
array(2) {
  [0]=>
  string(4) "--- "
  [1]=>
  string(60) " --- |1| --- aaa bbb ccc ddd --- |2| --- aaa bbb ccc ddd ---"
}

--> Pattern: '\\\`\^\.\[\$\(\)\|\*\+\?\{\''; match: '\`^.[$()|*+?{''
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(38) " |1| \`^.[$()|*+?{' |2| \`^.[$()|*+?{'"
}

--> Pattern: '\a'; match: 'a'
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(12) " |1| a |2| a"
}

--> Pattern: '[0-9][^0-9]'; match: '2a'
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(14) " |1| 2a |2| 2a"
}

--> Pattern: '^[[:alnum:]]{62,62}$'; match: '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ'
array(1) {
  [0]=>
  string(196) "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ |1| 0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ |2| 0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
}

--> Pattern: '^[[:digit:]]{5}'; match: '0123456789'
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(35) "56789 |1| 0123456789 |2| 0123456789"
}

--> Pattern: '[[:digit:]]{5}$'; match: '0123456789'
array(2) {
  [0]=>
  string(35) "0123456789 |1| 0123456789 |2| 01234"
  [1]=>
  string(0) ""
}

--> Pattern: '[[:blank:]]{1,10}'; match: '
 	'
array(2) {
  [0]=>
  string(1) "
"
  [1]=>
  string(15) "|1| 
 	 |2| 
 	"
}

--> Pattern: '[[:print:]]{3}'; match: ' a '
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(16) " |1|  a  |2|  a "
}
Done
