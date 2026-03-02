--TEST--
Test array_push() function : usage variations - array keys are different data types
--FILE--
<?php
/*
 * Pass array_push arrays where the keys are different data types.
 */

echo "*** Testing array_push() : usage variations ***\n";

// Initialise function arguments not being substituted
$var = 'value';

// heredoc string
$heredoc = <<<EOT
hello world
EOT;

// arrays of different data types as keys to be passed to $stack argument
$inputs = array(

       // int data
/*1*/  'int' => array(
       0 => 'zero',
       1 => 'one',
       12345 => 'positive',
       -2345 => 'negative',
       ),

       // boolean data
/*4*/ 'bool lowercase' => array(
       true => 'lowert',
       false => 'lowerf',
       ),

       // empty data
       'empty single quotes' => array(
       '' => 'emptys',
       ),

       // string data
/*6*/ 'string' => array(
       "stringd" => 'stringd',
       'strings' => 'strings',
       $heredoc => 'stringh',
       ),
);

// loop through each sub-array of $inputs to check the behavior of array_push()
foreach($inputs as $key => $input) {
  echo "\n-- $key data --\n";
  echo "Before : ";
  var_dump(count($input));
  echo "After  : ";
  var_dump( array_push($input, $var) );
};

echo "Done";
?>
--EXPECT--
*** Testing array_push() : usage variations ***

-- int data --
Before : int(4)
After  : int(5)

-- bool lowercase data --
Before : int(2)
After  : int(3)

-- empty single quotes data --
Before : int(1)
After  : int(2)

-- string data --
Before : int(3)
After  : int(4)
Done
