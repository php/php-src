--TEST--
Test scandir() function : usage variations - diff data types as $context arg
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
  die("skip Valid only on Windows");
}
?>
--FILE--
<?php
/* Prototype  : array scandir(string $dir [, int $sorting_order [, resource $context]])
 * Description: List files & directories inside the specified path
 * Source code: ext/standard/dir.c
 */

/*
 * Pass different data types as $context argument to test how scandir() behaves
 */

echo "*** Testing scandir() : usage variations ***\n";

// Initialise function arguments not being substituted
$dir = dirname(__FILE__) . '/私はガラスを食べられますscandir_variation3';
mkdir($dir);
$sorting_order = SCANDIR_SORT_ASCENDING;

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

// unexpected values to be passed to $context argument
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

// loop through each element of $inputs to check the behavior of scandir()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( scandir($dir, $sorting_order, $input) );
  $iterator++;
};

fclose($fp);
?>
===DONE===
--CLEAN--
<?php
$dir = dirname(__FILE__) . '/私はガラスを食べられますscandir_variation3';
rmdir($dir);
?>
--EXPECTF--
*** Testing scandir() : usage variations ***

-- Iteration 1 --

Warning: scandir() expects parameter 3 to be resource, int given in %s on line %d
NULL

-- Iteration 2 --

Warning: scandir() expects parameter 3 to be resource, int given in %s on line %d
NULL

-- Iteration 3 --

Warning: scandir() expects parameter 3 to be resource, int given in %s on line %d
NULL

-- Iteration 4 --

Warning: scandir() expects parameter 3 to be resource, int given in %s on line %d
NULL

-- Iteration 5 --

Warning: scandir() expects parameter 3 to be resource, float given in %s on line %d
NULL

-- Iteration 6 --

Warning: scandir() expects parameter 3 to be resource, float given in %s on line %d
NULL

-- Iteration 7 --

Warning: scandir() expects parameter 3 to be resource, float given in %s on line %d
NULL

-- Iteration 8 --

Warning: scandir() expects parameter 3 to be resource, float given in %s on line %d
NULL

-- Iteration 9 --

Warning: scandir() expects parameter 3 to be resource, float given in %s on line %d
NULL

-- Iteration 10 --

Warning: scandir() expects parameter 3 to be resource, null given in %s on line %d
NULL

-- Iteration 11 --

Warning: scandir() expects parameter 3 to be resource, null given in %s on line %d
NULL

-- Iteration 12 --

Warning: scandir() expects parameter 3 to be resource, bool given in %s on line %d
NULL

-- Iteration 13 --

Warning: scandir() expects parameter 3 to be resource, bool given in %s on line %d
NULL

-- Iteration 14 --

Warning: scandir() expects parameter 3 to be resource, bool given in %s on line %d
NULL

-- Iteration 15 --

Warning: scandir() expects parameter 3 to be resource, bool given in %s on line %d
NULL

-- Iteration 16 --

Warning: scandir() expects parameter 3 to be resource, string given in %s on line %d
NULL

-- Iteration 17 --

Warning: scandir() expects parameter 3 to be resource, string given in %s on line %d
NULL

-- Iteration 18 --

Warning: scandir() expects parameter 3 to be resource, array given in %s on line %d
NULL

-- Iteration 19 --

Warning: scandir() expects parameter 3 to be resource, string given in %s on line %d
NULL

-- Iteration 20 --

Warning: scandir() expects parameter 3 to be resource, string given in %s on line %d
NULL

-- Iteration 21 --

Warning: scandir() expects parameter 3 to be resource, string given in %s on line %d
NULL

-- Iteration 22 --

Warning: scandir() expects parameter 3 to be resource, object given in %s on line %d
NULL

-- Iteration 23 --

Warning: scandir() expects parameter 3 to be resource, null given in %s on line %d
NULL

-- Iteration 24 --

Warning: scandir() expects parameter 3 to be resource, null given in %s on line %d
NULL

-- Iteration 25 --

Warning: scandir(): supplied resource is not a valid Stream-Context resource in %s on line %d
array(2) {
  [0]=>
  string(1) "."
  [1]=>
  string(2) ".."
}
===DONE===
