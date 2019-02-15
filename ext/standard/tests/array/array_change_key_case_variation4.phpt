--TEST--
Test array_change_key_case() function : usage variations - different int values for $case
--FILE--
<?php
/* Prototype  : array array_change_key_case(array $input [, int $case])
 * Description: Returns an array with all string keys lowercased [or uppercased]
 * Source code: ext/standard/array.c
 */

/*
 * Pass different integer values as $case argument to array_change_key_case() to test behaviour
 */

echo "*** Testing array_change_key_case() : usage variations ***\n";

$input = array('One' => 'un', 'TWO' => 'deux', 'three' => 'trois');
for ($i = -5; $i <=5; $i += 1){
	echo "\n-- \$sort argument is $i --\n";
	$temp = $input;
	var_dump(array_change_key_case($temp, $i));
}

echo "Done";
?>
--EXPECT--
*** Testing array_change_key_case() : usage variations ***

-- $sort argument is -5 --
array(3) {
  ["ONE"]=>
  string(2) "un"
  ["TWO"]=>
  string(4) "deux"
  ["THREE"]=>
  string(5) "trois"
}

-- $sort argument is -4 --
array(3) {
  ["ONE"]=>
  string(2) "un"
  ["TWO"]=>
  string(4) "deux"
  ["THREE"]=>
  string(5) "trois"
}

-- $sort argument is -3 --
array(3) {
  ["ONE"]=>
  string(2) "un"
  ["TWO"]=>
  string(4) "deux"
  ["THREE"]=>
  string(5) "trois"
}

-- $sort argument is -2 --
array(3) {
  ["ONE"]=>
  string(2) "un"
  ["TWO"]=>
  string(4) "deux"
  ["THREE"]=>
  string(5) "trois"
}

-- $sort argument is -1 --
array(3) {
  ["ONE"]=>
  string(2) "un"
  ["TWO"]=>
  string(4) "deux"
  ["THREE"]=>
  string(5) "trois"
}

-- $sort argument is 0 --
array(3) {
  ["one"]=>
  string(2) "un"
  ["two"]=>
  string(4) "deux"
  ["three"]=>
  string(5) "trois"
}

-- $sort argument is 1 --
array(3) {
  ["ONE"]=>
  string(2) "un"
  ["TWO"]=>
  string(4) "deux"
  ["THREE"]=>
  string(5) "trois"
}

-- $sort argument is 2 --
array(3) {
  ["ONE"]=>
  string(2) "un"
  ["TWO"]=>
  string(4) "deux"
  ["THREE"]=>
  string(5) "trois"
}

-- $sort argument is 3 --
array(3) {
  ["ONE"]=>
  string(2) "un"
  ["TWO"]=>
  string(4) "deux"
  ["THREE"]=>
  string(5) "trois"
}

-- $sort argument is 4 --
array(3) {
  ["ONE"]=>
  string(2) "un"
  ["TWO"]=>
  string(4) "deux"
  ["THREE"]=>
  string(5) "trois"
}

-- $sort argument is 5 --
array(3) {
  ["ONE"]=>
  string(2) "un"
  ["TWO"]=>
  string(4) "deux"
  ["THREE"]=>
  string(5) "trois"
}
Done
