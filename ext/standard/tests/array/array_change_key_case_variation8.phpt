--TEST--
Test array_change_key_case() function : usage variations - Different strings as keys
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
  die("skip Output tested contains chars that are not shown the same on windows concole (ESC and co)");
}
--FILE--
<?php
/* Prototype  : array array_change_key_case(array $input [, int $case])
 * Description: Retuns an array with all string keys lowercased [or uppercased]
 * Source code: ext/standard/array.c
 */

/*
 * Test how array_change_key_case() behaves with different strings
 */

echo "*** Testing array_change_key_case() : usage variations ***\n";

$inputs = array (
	// group of escape sequences
	array(null => 1, NULL => 2, "\a" => 3, "\cx" => 4, "\e" => 5, "\f" => 6, "\n" => 7, "\t" => 8, "\xhh" => 9, "\ddd" => 10, "\v" => 11),

	// array contains combination of capital/small letters
	array("lemoN" => 1, "Orange" => 2, "banana" => 3, "apple" => 4, "Test" => 5, "TTTT" => 6, "ttt" => 7, "ww" => 8, "x" => 9, "X" => 10, "oraNGe" => 11, "BANANA" => 12)
);

foreach($inputs as $input) {
	echo "\n-- \$case = default --\n";
	var_dump(array_change_key_case($input));
	echo "-- \$case = upper --\n";
	var_dump(array_change_key_case($input, CASE_UPPER));
}

echo "Done";
?>
--EXPECTF--
*** Testing array_change_key_case() : usage variations ***

-- $case = default --
array(10) {
  [""]=>
  int(2)
  ["\a"]=>
  int(3)
  ["\cx"]=>
  int(4)
  [""]=>
  int(5)
  [""]=>
  int(6)
  ["
"]=>
  int(7)
  ["	"]=>
  int(8)
  ["\xhh"]=>
  int(9)
  ["\ddd"]=>
  int(10)
  [""]=>
  int(11)
}
-- $case = upper --
array(10) {
  [""]=>
  int(2)
  ["\A"]=>
  int(3)
  ["\CX"]=>
  int(4)
  [""]=>
  int(5)
  [""]=>
  int(6)
  ["
"]=>
  int(7)
  ["	"]=>
  int(8)
  ["\XHH"]=>
  int(9)
  ["\DDD"]=>
  int(10)
  [""]=>
  int(11)
}

-- $case = default --
array(9) {
  ["lemon"]=>
  int(1)
  ["orange"]=>
  int(11)
  ["banana"]=>
  int(12)
  ["apple"]=>
  int(4)
  ["test"]=>
  int(5)
  ["tttt"]=>
  int(6)
  ["ttt"]=>
  int(7)
  ["ww"]=>
  int(8)
  ["x"]=>
  int(10)
}
-- $case = upper --
array(9) {
  ["LEMON"]=>
  int(1)
  ["ORANGE"]=>
  int(11)
  ["BANANA"]=>
  int(12)
  ["APPLE"]=>
  int(4)
  ["TEST"]=>
  int(5)
  ["TTTT"]=>
  int(6)
  ["TTT"]=>
  int(7)
  ["WW"]=>
  int(8)
  ["X"]=>
  int(10)
}
Done
