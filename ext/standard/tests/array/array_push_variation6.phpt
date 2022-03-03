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

//get an unset variable
$unset_var = 10;
unset ($unset_var);

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

       // null data
/*3*/ 'null uppercase' => array(
       NULL => 'null 1',
       ),
       'null lowercase' => array(
       null => 'null 2',
       ),

       // boolean data
/*4*/ 'bool lowercase' => array(
       true => 'lowert',
       false => 'lowerf',
       ),
       'bool uppercase' => array(
       TRUE => 'uppert',
       FALSE => 'upperf',
       ),

       // empty data
/*5*/ 'empty double quotes' => array(
       "" => 'emptyd',
       ),
       'empty single quotes' => array(
       '' => 'emptys',
       ),

       // string data
/*6*/ 'string' => array(
       "stringd" => 'stringd',
       'strings' => 'strings',
       $heredoc => 'stringh',
       ),

       // undefined data
/*8*/ 'undefined' => array(
       @$undefined_var => 'undefined',
       ),

       // unset data
/*9*/ 'unset' => array(
       @$unset_var => 'unset',
       ),
);

// loop through each sub-array of $inputs to check the behavior of array_push()
$iterator = 1;
foreach($inputs as $key => $input) {
  echo "\n-- Iteration $iterator : $key data --\n";
  echo "Before : ";
  var_dump(count($input));
  echo "After  : ";
  var_dump( array_push($input, $var) );
  $iterator++;
};

echo "Done";
?>
--EXPECT--
*** Testing array_push() : usage variations ***

-- Iteration 1 : int data --
Before : int(4)
After  : int(5)

-- Iteration 2 : null uppercase data --
Before : int(1)
After  : int(2)

-- Iteration 3 : null lowercase data --
Before : int(1)
After  : int(2)

-- Iteration 4 : bool lowercase data --
Before : int(2)
After  : int(3)

-- Iteration 5 : bool uppercase data --
Before : int(2)
After  : int(3)

-- Iteration 6 : empty double quotes data --
Before : int(1)
After  : int(2)

-- Iteration 7 : empty single quotes data --
Before : int(1)
After  : int(2)

-- Iteration 8 : string data --
Before : int(3)
After  : int(4)

-- Iteration 9 : undefined data --
Before : int(1)
After  : int(2)

-- Iteration 10 : unset data --
Before : int(1)
After  : int(2)
Done
