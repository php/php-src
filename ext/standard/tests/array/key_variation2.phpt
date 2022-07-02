--TEST--
Test key() function : usage variations
--FILE--
<?php
/*
 * Pass arrays where keys are different data types as $array_arg to key() to test behaviour
 */

echo "*** Testing key() : usage variations ***\n";

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// heredoc string
$heredoc = <<<EOT
hello world
EOT;

// unexpected values to be passed as $array_arg
$inputs = array(

       // int data
/*1*/  'int' => array(
       0 => 'zero',
       1 => 'one',
       12345 => 'positive',
       -2345 => 'negative',
       ),

       // null data
/*4*/ 'null uppercase' => array(
       NULL => 'null 1',
       ),

/*5*/  'null lowercase' => array(
       null => 'null 2',
       ),

       // boolean data
/*6*/ 'bool lowercase' => array(
       true => 'lowert',
       false => 'lowerf',
       ),

/*7*/  'bool uppercase' => array(
       TRUE => 'uppert',
       FALSE => 'upperf',
       ),

       // empty data
/*8*/ 'empty double quotes' => array(
       "" => 'emptyd',
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

       // undefined data
/*11*/ 'undefined' => array(
       @$undefined_var => 'undefined',
       ),

       // unset data
/*12*/ 'unset' => array(
       @$unset_var => 'unset',
       ),
);

// loop through each element of $inputs to check the behavior of key()
$iterator = 1;
foreach($inputs as $key => $input) {
  echo "\n-- Iteration $iterator : $key data --\n";
  while (key($input) !== NULL) {
    var_dump(key($input));
    next($input);
  }
  $iterator++;
};
?>
--EXPECT--
*** Testing key() : usage variations ***

-- Iteration 1 : int data --
int(0)
int(1)
int(12345)
int(-2345)

-- Iteration 2 : null uppercase data --
string(0) ""

-- Iteration 3 : null lowercase data --
string(0) ""

-- Iteration 4 : bool lowercase data --
int(1)
int(0)

-- Iteration 5 : bool uppercase data --
int(1)
int(0)

-- Iteration 6 : empty double quotes data --
string(0) ""

-- Iteration 7 : empty single quotes data --
string(0) ""

-- Iteration 8 : string data --
string(7) "stringd"
string(7) "strings"
string(11) "hello world"

-- Iteration 9 : undefined data --
string(0) ""

-- Iteration 10 : unset data --
string(0) ""
