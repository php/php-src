--TEST--
Test array_change_key_case() function : usage variations - referenced variables
--FILE--
<?php
/* Prototype  : array array_change_key_case(array $input [, int $case])
 * Description: Retuns an array with all string keys lowercased [or uppercased] 
 * Source code: ext/standard/array.c
 */

/*
 * Test array_change_key_case() when:
 * 1. Passed a referenced variable
 * 2. Passed an argument by reference
 */

echo "*** Testing array_change_key_case() : usage variations ***\n";

$input = array('one' => 1, 'two' => 2, 'ABC' => 'xyz');

echo "\n-- \$input argument is a reference to array --\n";
$new_input = &$input;
echo "Result:\n";
var_dump(array_change_key_case($new_input, CASE_UPPER));
echo "Original:\n";
var_dump($input);
echo "Referenced:\n";
var_dump($new_input);

echo "\n-- \$input is an array passed by reference --\n";
echo "Result:\n";
var_dump(array_change_key_case(&$input, CASE_UPPER));
echo "Original:\n";
var_dump($input);

echo "Done";
?>

--EXPECTF--
Deprecated: Call-time pass-by-reference has been deprecated in %s on line %d
*** Testing array_change_key_case() : usage variations ***

-- $input argument is a reference to array --
Result:
array(3) {
  [u"ONE"]=>
  int(1)
  [u"TWO"]=>
  int(2)
  [u"ABC"]=>
  unicode(3) "xyz"
}
Original:
array(3) {
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
  [u"ABC"]=>
  unicode(3) "xyz"
}
Referenced:
array(3) {
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
  [u"ABC"]=>
  unicode(3) "xyz"
}

-- $input is an array passed by reference --
Result:
array(3) {
  [u"ONE"]=>
  int(1)
  [u"TWO"]=>
  int(2)
  [u"ABC"]=>
  unicode(3) "xyz"
}
Original:
array(3) {
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
  [u"ABC"]=>
  unicode(3) "xyz"
}
Done
