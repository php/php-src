--TEST--
Test spliti() function : basic functionality - test a number of simple spliti, without specifying a limit
--FILE--
<?php
/* Prototype  : proto array spliti(string pattern, string string [, int limit])
 * Description: spliti string into array by regular expression 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

/*
 * Test a number of simple spliti, without specifying a limit
 */

echo "*** Testing ereg() : basic functionality ***\n";

include(dirname(__FILE__) . '/regular_expressions.inc');

foreach ($expressions as $re) {
	list($pattern,$string) = $re;
	echo "\n--> Pattern: '$pattern'; match: '$string'\n";
	var_dump(spliti($pattern, $string . ' |1| ' . $string . ' |2| ' . $string));
}

echo "Done";
?>
--EXPECTF--
*** Testing ereg() : basic functionality ***

--> Pattern: '..(a|b|c)(a|b|c)..'; match: '--- ab ---'
array(4) {
  [0]=>
  string(2) "--"
  [1]=>
  string(9) "-- |1| --"
  [2]=>
  string(9) "-- |2| --"
  [3]=>
  string(2) "--"
}

--> Pattern: '()'; match: ''

Warning: spliti(): Invalid Regular Expression in %s on line 19
bool(false)

--> Pattern: '()'; match: 'abcdef'

Warning: spliti(): Invalid Regular Expression in %s on line 19
bool(false)

--> Pattern: '[x]|[^x]'; match: 'abcdef'
array(29) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
  [2]=>
  string(0) ""
  [3]=>
  string(0) ""
  [4]=>
  string(0) ""
  [5]=>
  string(0) ""
  [6]=>
  string(0) ""
  [7]=>
  string(0) ""
  [8]=>
  string(0) ""
  [9]=>
  string(0) ""
  [10]=>
  string(0) ""
  [11]=>
  string(0) ""
  [12]=>
  string(0) ""
  [13]=>
  string(0) ""
  [14]=>
  string(0) ""
  [15]=>
  string(0) ""
  [16]=>
  string(0) ""
  [17]=>
  string(0) ""
  [18]=>
  string(0) ""
  [19]=>
  string(0) ""
  [20]=>
  string(0) ""
  [21]=>
  string(0) ""
  [22]=>
  string(0) ""
  [23]=>
  string(0) ""
  [24]=>
  string(0) ""
  [25]=>
  string(0) ""
  [26]=>
  string(0) ""
  [27]=>
  string(0) ""
  [28]=>
  string(0) ""
}

--> Pattern: '(a{1})(a{1,}) (b{1,3}) (c+) (d?ddd|e)'; match: '--- aaa bbb ccc ddd ---'
array(4) {
  [0]=>
  string(4) "--- "
  [1]=>
  string(13) " --- |1| --- "
  [2]=>
  string(13) " --- |2| --- "
  [3]=>
  string(4) " ---"
}

--> Pattern: '\\\`\^\.\[\$\(\)\|\*\+\?\{\''; match: '\`^.[$()|*+?{''
array(4) {
  [0]=>
  string(0) ""
  [1]=>
  string(5) " |1| "
  [2]=>
  string(5) " |2| "
  [3]=>
  string(0) ""
}

--> Pattern: '\a'; match: 'a'
array(4) {
  [0]=>
  string(0) ""
  [1]=>
  string(5) " |1| "
  [2]=>
  string(5) " |2| "
  [3]=>
  string(0) ""
}

--> Pattern: '[0-9][^0-9]'; match: '2a'
array(6) {
  [0]=>
  string(0) ""
  [1]=>
  string(2) " |"
  [2]=>
  string(1) " "
  [3]=>
  string(2) " |"
  [4]=>
  string(1) " "
  [5]=>
  string(0) ""
}

--> Pattern: '^[[:alnum:]]{62,62}$'; match: '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ'
array(1) {
  [0]=>
  string(196) "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ |1| 0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ |2| 0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
}

--> Pattern: '^[[:digit:]]{5}'; match: '0123456789'
array(3) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
  [2]=>
  string(30) " |1| 0123456789 |2| 0123456789"
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
array(6) {
  [0]=>
  string(1) "
"
  [1]=>
  string(3) "|1|"
  [2]=>
  string(1) "
"
  [3]=>
  string(3) "|2|"
  [4]=>
  string(1) "
"
  [5]=>
  string(0) ""
}

--> Pattern: '[[:print:]]{3}'; match: ' a '
array(7) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
  [2]=>
  string(0) ""
  [3]=>
  string(0) ""
  [4]=>
  string(0) ""
  [5]=>
  string(0) ""
  [6]=>
  string(1) " "
}
Done
