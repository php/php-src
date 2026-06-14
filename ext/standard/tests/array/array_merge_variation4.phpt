--TEST--
Test array_merge() function : usage variations - Diff. data types as array keys
--FILE--
<?php
/*
 * Pass an array with different data types as keys to test how array_merge
 * adds it onto an existing array
 */

echo "*** Testing array_merge() : usage variations ***\n";

// Initialise function arguments not being substituted
$arr = array ('one' => 1, 'two' => 2);

// heredoc string
$heredoc = <<<EOT
hello world
EOT;

// arrays with keys as different data types to be passed as $input
$inputs = array(

       // int data
/*1*/  'int' => array(
       0 => 'zero',
       1 => 'one',
       12345 => 'positive',
       -2345 => 'negative',
       ),

       // boolean data
/*6*/ 'bool lowercase' => array(
       true => 'lowert',
       false => 'lowerf',
       ),

       // empty data
/*9*/  'empty single quotes' => array(
       '' => 'emptys',
       ),

       // string data
/*10*/ 'string' => array(
       "stringd" => 'stringd',
       'strings' => 'strings',
       $heredoc => 'stringh',
       ),
);

// loop through each element of $inputs to check the behavior of array_merge
$iterator = 1;
foreach($inputs as $key => $input) {
  echo "\n-- Iteration $iterator: $key data --\n";
  var_dump( array_merge($input, $arr) );
  var_dump( array_merge($arr, $input) );
  $iterator++;
};

echo "Done";
?>
--EXPECT--
*** Testing array_merge() : usage variations ***

-- Iteration 1: int data --
array(6) {
  [0]=>
  string(4) "zero"
  [1]=>
  string(3) "one"
  [2]=>
  string(8) "positive"
  [3]=>
  string(8) "negative"
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(6) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  [0]=>
  string(4) "zero"
  [1]=>
  string(3) "one"
  [2]=>
  string(8) "positive"
  [3]=>
  string(8) "negative"
}

-- Iteration 2: bool lowercase data --
array(4) {
  [0]=>
  string(6) "lowert"
  [1]=>
  string(6) "lowerf"
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(4) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  [0]=>
  string(6) "lowert"
  [1]=>
  string(6) "lowerf"
}

-- Iteration 3: empty single quotes data --
array(3) {
  [""]=>
  string(6) "emptys"
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(3) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  [""]=>
  string(6) "emptys"
}

-- Iteration 4: string data --
array(5) {
  ["stringd"]=>
  string(7) "stringd"
  ["strings"]=>
  string(7) "strings"
  ["hello world"]=>
  string(7) "stringh"
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(5) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  ["stringd"]=>
  string(7) "stringd"
  ["strings"]=>
  string(7) "strings"
  ["hello world"]=>
  string(7) "stringh"
}
Done
