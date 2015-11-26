--TEST--
Test each() function : usage variations - Pass different data types as $arr arg
--FILE--
<?php
/* Prototype  : array each(array $arr)
 * Description: Return the currently pointed key..value pair in the passed array,
 * and advance the pointer to the next element 
 * Source code: Zend/zend_builtin_functions.c
 */

/*
 * Pass different data types as $arr arg to each() to test behaviour
 */

echo "*** Testing each() : usage variations ***\n";

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

// unexpected values to be passed to $arr argument
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
       array(),

       // string data
/*19*/ "string",
       'string',
       $heredoc,
       
       // object data
/*22*/ new classA(),

       // undefined data
/*23*/ @$undefined_var,

       // unset data
/*24*/ @$unset_var,

       // resource variable
/*25*/ $fp
);

// loop through each element of $inputs to check the behavior of each()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( each($input) );
  $iterator++;
};

fclose($fp);

echo "Done";
?>

--EXPECTF--
*** Testing each() : usage variations ***

-- Iteration 1 --

Warning: Variable passed to each() is not an array or object in %s on line %d
NULL

-- Iteration 2 --

Warning: Variable passed to each() is not an array or object in %s on line %d
NULL

-- Iteration 3 --

Warning: Variable passed to each() is not an array or object in %s on line %d
NULL

-- Iteration 4 --

Warning: Variable passed to each() is not an array or object in %s on line %d
NULL

-- Iteration 5 --

Warning: Variable passed to each() is not an array or object in %s on line %d
NULL

-- Iteration 6 --

Warning: Variable passed to each() is not an array or object in %s on line %d
NULL

-- Iteration 7 --

Warning: Variable passed to each() is not an array or object in %s on line %d
NULL

-- Iteration 8 --

Warning: Variable passed to each() is not an array or object in %s on line %d
NULL

-- Iteration 9 --

Warning: Variable passed to each() is not an array or object in %s on line %d
NULL

-- Iteration 10 --

Warning: Variable passed to each() is not an array or object in %s on line %d
NULL

-- Iteration 11 --

Warning: Variable passed to each() is not an array or object in %s on line %d
NULL

-- Iteration 12 --

Warning: Variable passed to each() is not an array or object in %s on line %d
NULL

-- Iteration 13 --

Warning: Variable passed to each() is not an array or object in %s on line %d
NULL

-- Iteration 14 --

Warning: Variable passed to each() is not an array or object in %s on line %d
NULL

-- Iteration 15 --

Warning: Variable passed to each() is not an array or object in %s on line %d
NULL

-- Iteration 16 --

Warning: Variable passed to each() is not an array or object in %s on line %d
NULL

-- Iteration 17 --

Warning: Variable passed to each() is not an array or object in %s on line %d
NULL

-- Iteration 18 --
bool(false)

-- Iteration 19 --

Warning: Variable passed to each() is not an array or object in %s on line %d
NULL

-- Iteration 20 --

Warning: Variable passed to each() is not an array or object in %s on line %d
NULL

-- Iteration 21 --

Warning: Variable passed to each() is not an array or object in %s on line %d
NULL

-- Iteration 22 --
bool(false)

-- Iteration 23 --

Warning: Variable passed to each() is not an array or object in %s on line %d
NULL

-- Iteration 24 --

Warning: Variable passed to each() is not an array or object in %s on line %d
NULL

-- Iteration 25 --

Warning: Variable passed to each() is not an array or object in %s on line %d
NULL
Done
