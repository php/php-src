--TEST--
Test count() function : usage variations - Pass different data types as $mode arg
--FILE--
<?php
/* Prototype  : int count(mixed $var [, int $mode])
 * Description: Count the number of elements in a variable (usually an array) 
 * Source code: ext/standard/array.c
 */

/*
 * Pass different data types as $mode argument to count() to test behaviour
 */

echo "*** Testing count() : usage variations ***\n";

// Initialise function arguments not being substituted
$var = array(1, 2, array ('one', 'two'));

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// get a class
class classA
{
  public function __toString() {
    return "Class A object";
  }
}

// heredoc string
$heredoc = <<<EOT
hello world
EOT;

// get a resource variable
$fp = fopen(__FILE__, "r");

// unexpected values to be passed to $mode argument
$inputs = array(

       // int data
/*1*/  0,
       1,
       12345,
       -2345,

       // float data
/*5*/  10.5,
       -10.5,
       12.3456789000e10,
       12.3456789000E-10,
       .5,

       // null data
/*10*/ NULL,
       null,

       // boolean data
/*12*/ true,
       false,
       TRUE,
       FALSE,
       
       // empty data
/*16*/ "",
       '',

       // string data
/*18*/ "string",
       'string',
       $heredoc,
       
       // object data
/*21*/ new classA(),

       // undefined data
/*22*/ @$undefined_var,

       // unset data
/*23*/ @$unset_var,

       // resource variable
/*24*/ $fp
);

// loop through each element of $inputs to check the behavior of count()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( count($var, $input) );
  $iterator++;
};

fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing count() : usage variations ***

-- Iteration 1 --
int(3)

-- Iteration 2 --
int(5)

-- Iteration 3 --
int(3)

-- Iteration 4 --
int(3)

-- Iteration 5 --
int(3)

-- Iteration 6 --
int(3)

-- Iteration 7 --
int(3)

-- Iteration 8 --
int(3)

-- Iteration 9 --
int(3)

-- Iteration 10 --
int(3)

-- Iteration 11 --
int(3)

-- Iteration 12 --
int(5)

-- Iteration 13 --
int(3)

-- Iteration 14 --
int(5)

-- Iteration 15 --
int(3)

-- Iteration 16 --

Warning: count() expects parameter 2 to be long, string given in %s on line %d
NULL

-- Iteration 17 --

Warning: count() expects parameter 2 to be long, string given in %s on line %d
NULL

-- Iteration 18 --

Warning: count() expects parameter 2 to be long, string given in %s on line %d
NULL

-- Iteration 19 --

Warning: count() expects parameter 2 to be long, string given in %s on line %d
NULL

-- Iteration 20 --

Warning: count() expects parameter 2 to be long, string given in %s on line %d
NULL

-- Iteration 21 --

Warning: count() expects parameter 2 to be long, object given in %s on line %d
NULL

-- Iteration 22 --
int(3)

-- Iteration 23 --
int(3)

-- Iteration 24 --

Warning: count() expects parameter 2 to be long, resource given in %s on line %d
NULL
Done