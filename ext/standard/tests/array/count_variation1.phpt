--TEST--
Test count() function : usage variations - Pass different data types as $var arg
--FILE--
<?php
/* Prototype  : int count(mixed $var [, int $mode])
 * Description: Count the number of elements in a variable (usually an array) 
 * Source code: ext/standard/array.c
 */

/*
 * aPass different data types as $var argument to count() to test behaviour
 */

echo "*** Testing count() : usage variations ***\n";

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

// unexpected values to be passed to $var argument
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
  var_dump( count($input) );
  $iterator++;
};

fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing count() : usage variations ***

-- Iteration 1 --
int(1)

-- Iteration 2 --
int(1)

-- Iteration 3 --
int(1)

-- Iteration 4 --
int(1)

-- Iteration 5 --
int(1)

-- Iteration 6 --
int(1)

-- Iteration 7 --
int(1)

-- Iteration 8 --
int(1)

-- Iteration 9 --
int(1)

-- Iteration 10 --
int(0)

-- Iteration 11 --
int(0)

-- Iteration 12 --
int(1)

-- Iteration 13 --
int(1)

-- Iteration 14 --
int(1)

-- Iteration 15 --
int(1)

-- Iteration 16 --
int(1)

-- Iteration 17 --
int(1)

-- Iteration 18 --
int(1)

-- Iteration 19 --
int(1)

-- Iteration 20 --
int(1)

-- Iteration 21 --
int(1)

-- Iteration 22 --
int(0)

-- Iteration 23 --
int(0)

-- Iteration 24 --
int(1)
Done