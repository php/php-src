--TEST--
Test array_shift() function : usage variations - Pass array with different data types as keys
--FILE--
<?php
/*
 * Pass arrays with different data types as keys to test how array_shift() re-assigns keys
 */

echo "*** Testing array_shift() : usage variations ***\n";

// heredoc string
$heredoc = <<<EOT
hello world
EOT;

// unexpected values to be passed to $stack argument
$inputs = array(

       // int data
/*1*/  'int' => array(
       0 => 'zero',
       1 => 'one',
       12345 => 'positive',
       -2345 => 'negative',
       ),

       // boolean data
/*6*/  'bool lowercase' => array(
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

// loop through each element of $inputs to check the behavior of array_shift()
$iterator = 1;
foreach($inputs as $key => $input) {
  echo "\n-- Iteration $iterator : $key data --\n";
  var_dump( array_shift($input) );
  var_dump($input);
  $iterator++;
};

echo "Done";
?>
--EXPECT--
*** Testing array_shift() : usage variations ***

-- Iteration 1 : int data --
string(4) "zero"
array(3) {
  [0]=>
  string(3) "one"
  [1]=>
  string(8) "positive"
  [2]=>
  string(8) "negative"
}

-- Iteration 2 : bool lowercase data --
string(6) "lowert"
array(1) {
  [0]=>
  string(6) "lowerf"
}

-- Iteration 3 : empty single quotes data --
string(6) "emptys"
array(0) {
}

-- Iteration 4 : string data --
string(7) "stringd"
array(2) {
  ["strings"]=>
  string(7) "strings"
  ["hello world"]=>
  string(7) "stringh"
}
Done
