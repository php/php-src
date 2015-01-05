--TEST--
Test current() function : usage variations - arrays containing different data types
--FILE--
<?php
/* Prototype  : mixed current(array $array_arg)
 * Description: Return the element currently pointed to by the internal array pointer
 * Source code: ext/standard/array.c
 * Alias to functions: pos
 */

/*
 * Pass arrays of different data types as $array_arg to current() to test behaviour
 */

echo "*** Testing current() : usage variations ***\n";

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

// arrays of different data types to be passed to $array_arg argument
$inputs = array(

       // int data
/*1*/  'int' => array(
	   0,
       1,
       12345,
       -2345,
       ),

       // float data
/*2*/  'float' => array(
       10.5,
       -10.5,
       12.3456789000e10,
       12.3456789000E-10,
       .5,
       ),

       // null data
/*3*/ 'null' => array(
       NULL,
       null,
       ),

       // boolean data
/*4*/ 'bool' => array(
       true,
       false,
       TRUE,
       FALSE,
       ),
       
       // empty data
/*5*/ 'empty string' => array(
       "",
       '',
       ),
       
/*6*/ 'empty array' => array(
       ),

       // string data
/*7*/ 'string' => array(
       "string",
       'string',
       $heredoc,
       ),
       
       // object data
/*8*/ 'object' => array(
       new classA(),
       ),

       // undefined data
/*9*/ 'undefined' => array(
       @$undefined_var,
       ),

       // unset data
/*10*/ 'unset' => array(
       @$unset_var,
       ),

       // resource variable
/*11*/ 'resource' => array(
       $fp
       ),
);

// loop through each element of $inputs to check the behavior of current()
$iterator = 1;
foreach($inputs as $key => $input) {
  echo "\n-- Iteration $iterator : $key data --\n";
  var_dump( current($input) );
  $iterator++;
};

fclose($fp);
?>
===DONE===
--EXPECTF--
*** Testing current() : usage variations ***

-- Iteration 1 : int data --
int(0)

-- Iteration 2 : float data --
float(10.5)

-- Iteration 3 : null data --
NULL

-- Iteration 4 : bool data --
bool(true)

-- Iteration 5 : empty string data --
string(0) ""

-- Iteration 6 : empty array data --
bool(false)

-- Iteration 7 : string data --
string(6) "string"

-- Iteration 8 : object data --
object(classA)#%d (0) {
}

-- Iteration 9 : undefined data --
NULL

-- Iteration 10 : unset data --
NULL

-- Iteration 11 : resource data --
resource(%d) of type (stream)
===DONE===
