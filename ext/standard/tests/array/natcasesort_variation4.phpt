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
--EXPECTF--
*** Testing natcasesort() : usage variation ***
bool(true)
array(11) {
  [0]=>
  NULL
  [1]=>
  NULL
  [7]=>
  unicode(1) "	"
  [6]=>
  unicode(1) "
"
  [10]=>
  unicode(1) ""
  [5]=>
  unicode(1) ""
  [2]=>
  unicode(2) "\a"
  [3]=>
  unicode(3) "\cx"
  [9]=>
  unicode(4) "\ddd"
  [4]=>
  unicode(2) "\e"
  [8]=>
  unicode(4) "\xhh"
}
bool(true)
array(12) {
  [3]=>
  unicode(5) "apple"
  [11]=>
  unicode(6) "BANANA"
  [2]=>
  unicode(6) "banana"
  [0]=>
  unicode(5) "lemoN"
  [10]=>
  unicode(6) "oraNGe"
  [1]=>
  unicode(6) "Orange"
  [4]=>
  unicode(4) "Test"
  [6]=>
  unicode(3) "ttt"
  [5]=>
  unicode(4) "TTTT"
  [7]=>
  unicode(2) "ww"
  [8]=>
  unicode(1) "x"
  [9]=>
  unicode(1) "X"
}
Done

