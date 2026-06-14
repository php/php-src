--TEST--
Test array_change_key_case() function : usage variations - different data types as keys
--FILE--
<?php
/*
 * Pass arrays with different data types as keys to array_change_key_case()
 * to test conversion
 */

echo "*** Testing array_change_key_case() : usage variations ***\n";

// arrays of different data types to be passed to $input argument
$inputs = [
	'int' => [
       0 => 'zero',
       1 => 'one',
       12345 => 'positive',
       -2345 => 'negative',
    ],
    'bool' => [
       true => 'true',
       false => 'false',
    ],
    'string' => [
       'strings' => 'strings',
       '' => 'emptys',
    ],
];

// loop through each sub-array of $inputs to check the behavior of array_change_key_case()
foreach($inputs as $key => $input) {
  echo "\n-- $key data --\n";
  var_dump( array_change_key_case($input, CASE_UPPER) );
};

echo "Done";
?>
--EXPECT--
*** Testing array_change_key_case() : usage variations ***

-- int data --
array(4) {
  [0]=>
  string(4) "zero"
  [1]=>
  string(3) "one"
  [12345]=>
  string(8) "positive"
  [-2345]=>
  string(8) "negative"
}

-- bool data --
array(2) {
  [1]=>
  string(4) "true"
  [0]=>
  string(5) "false"
}

-- string data --
array(2) {
  ["STRINGS"]=>
  string(7) "strings"
  [""]=>
  string(6) "emptys"
}
Done
