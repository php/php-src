--TEST--
Test array_change_key_case() function : usage variations - Different strings as keys
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
  [u""]=>
  int(2)
  [u"\a"]=>
  int(3)
  [u"\cx"]=>
  int(4)
  [u"\e"]=>
  int(5)
  [u""]=>
  int(6)
  [u"
"]=>
  int(7)
  [u"	"]=>
  int(8)
  [u"\xhh"]=>
  int(9)
  [u"\ddd"]=>
  int(10)
  [u""]=>
  int(11)
}
-- $case = upper --
array(10) {
  [u""]=>
  int(2)
  [u"\A"]=>
  int(3)
  [u"\CX"]=>
  int(4)
  [u"\E"]=>
  int(5)
  [u""]=>
  int(6)
  [u"
"]=>
  int(7)
  [u"	"]=>
  int(8)
  [u"\XHH"]=>
  int(9)
  [u"\DDD"]=>
  int(10)
  [u""]=>
  int(11)
}

-- $case = default --
array(9) {
  [u"lemon"]=>
  int(1)
  [u"orange"]=>
  int(11)
  [u"banana"]=>
  int(12)
  [u"apple"]=>
  int(4)
  [u"test"]=>
  int(5)
  [u"tttt"]=>
  int(6)
  [u"ttt"]=>
  int(7)
  [u"ww"]=>
  int(8)
  [u"x"]=>
  int(10)
}
-- $case = upper --
array(9) {
  [u"LEMON"]=>
  int(1)
  [u"ORANGE"]=>
  int(11)
  [u"BANANA"]=>
  int(12)
  [u"APPLE"]=>
  int(4)
  [u"TEST"]=>
  int(5)
  [u"TTTT"]=>
  int(6)
  [u"TTT"]=>
  int(7)
  [u"WW"]=>
  int(8)
  [u"X"]=>
  int(10)
}
Done
