--TEST--
Test natcasesort() function : usage variations - different string types
--FILE--
<?php
/* Prototype  : bool natcasesort(array &$array_arg)
 * Description: Sort an array using case-insensitive natural sort
 * Source code: ext/standard/array.c
 */

/*
 * Pass arrays of string data to see how natcasesort() re-orders the array
 */

echo "*** Testing natcasesort() : usage variation ***\n";

$inputs = array (
	// group of escape sequences
	array(null, NULL, "\a", "\cx", "\e", "\f", "\n", "\t", "\xhh", "\ddd", "\v"),

	// array contains combination of capital/small letters
	array("lemoN", "Orange", "banana", "apple", "Test", "TTTT", "ttt", "ww", "x", "X", "oraNGe", "BANANA")
);

foreach ($inputs as $array_arg) {
	var_dump( natcasesort($array_arg) );
	var_dump($array_arg);
}

echo "Done";
?>
--EXPECT--
*** Testing natcasesort() : usage variation ***
bool(true)
array(11) {
  [0]=>
  NULL
  [1]=>
  NULL
  [5]=>
  string(1) ""
  [6]=>
  string(1) "
"
  [7]=>
  string(1) "	"
  [10]=>
  string(1) ""
  [4]=>
  string(1) ""
  [2]=>
  string(2) "\a"
  [3]=>
  string(3) "\cx"
  [9]=>
  string(4) "\ddd"
  [8]=>
  string(4) "\xhh"
}
bool(true)
array(12) {
  [3]=>
  string(5) "apple"
  [2]=>
  string(6) "banana"
  [11]=>
  string(6) "BANANA"
  [0]=>
  string(5) "lemoN"
  [1]=>
  string(6) "Orange"
  [10]=>
  string(6) "oraNGe"
  [4]=>
  string(4) "Test"
  [6]=>
  string(3) "ttt"
  [5]=>
  string(4) "TTTT"
  [7]=>
  string(2) "ww"
  [8]=>
  string(1) "x"
  [9]=>
  string(1) "X"
}
Done
