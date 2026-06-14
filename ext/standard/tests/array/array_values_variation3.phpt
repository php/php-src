--TEST--
Test array_values() function : usage variations - array keys different data types
--FILE--
<?php
/*
 * Pass arrays where the keys are different data types as $input argument
 * to array_values() to test behaviour
 */

echo "*** Testing array_values() : usage variations ***\n";

// heredoc string
$heredoc = <<<EOT
hello world
EOT;

// unexpected values to be passed as $input
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

// loop through each element of $inputs to check the behavior of array_values()
foreach($inputs as $key => $input) {
  echo "\n-- $key data --\n";
  var_dump( array_values($input) );
};
echo "Done";
?>
--EXPECT--
*** Testing array_values() : usage variations ***

-- int data --
array(4) {
  [0]=>
  string(4) "zero"
  [1]=>
  string(3) "one"
  [2]=>
  string(8) "positive"
  [3]=>
  string(8) "negative"
}

-- bool lowercase data --
array(2) {
  [0]=>
  string(6) "lowert"
  [1]=>
  string(6) "lowerf"
}

-- empty single quotes data --
array(1) {
  [0]=>
  string(6) "emptys"
}

-- string data --
array(3) {
  [0]=>
  string(7) "stringd"
  [1]=>
  string(7) "strings"
  [2]=>
  string(7) "stringh"
}
Done
