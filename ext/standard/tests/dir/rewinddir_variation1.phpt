--TEST--
Test rewinddir() function : usage variations - different data types as $dir_handle arg
--FILE--
<?php
/* Prototype  : void rewinddir([resource $dir_handle])
 * Description: Rewind dir_handle back to the start 
 * Source code: ext/standard/dir.c
 * Alias to functions: rewind
 */

/*
 * Pass different data types as $dir_handle argument to rewinddir() to test behaviour
 */

echo "*** Testing rewinddir() : usage variations ***\n";

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

// unexpected values to be passed to $dir_handle argument
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
);

// loop through each element of $inputs to check the behavior of rewinddir()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( rewinddir($input) );
  $iterator++;
};
?>
===DONE===
--EXPECTF--
*** Testing rewinddir() : usage variations ***

-- Iteration 1 --

Warning: rewinddir(): supplied argument is not a valid Directory resource in %s on line %d
bool(false)

-- Iteration 2 --

Warning: rewinddir(): supplied argument is not a valid Directory resource in %s on line %d
bool(false)

-- Iteration 3 --

Warning: rewinddir(): supplied argument is not a valid Directory resource in %s on line %d
bool(false)

-- Iteration 4 --

Warning: rewinddir(): supplied argument is not a valid Directory resource in %s on line %d
bool(false)

-- Iteration 5 --

Warning: rewinddir(): supplied argument is not a valid Directory resource in %s on line %d
bool(false)

-- Iteration 6 --

Warning: rewinddir(): supplied argument is not a valid Directory resource in %s on line %d
bool(false)

-- Iteration 7 --

Warning: rewinddir(): supplied argument is not a valid Directory resource in %s on line %d
bool(false)

-- Iteration 8 --

Warning: rewinddir(): supplied argument is not a valid Directory resource in %s on line %d
bool(false)

-- Iteration 9 --

Warning: rewinddir(): supplied argument is not a valid Directory resource in %s on line %d
bool(false)

-- Iteration 10 --

Warning: rewinddir(): supplied argument is not a valid Directory resource in %s on line %d
bool(false)

-- Iteration 11 --

Warning: rewinddir(): supplied argument is not a valid Directory resource in %s on line %d
bool(false)

-- Iteration 12 --

Warning: rewinddir(): supplied argument is not a valid Directory resource in %s on line %d
bool(false)

-- Iteration 13 --

Warning: rewinddir(): supplied argument is not a valid Directory resource in %s on line %d
bool(false)

-- Iteration 14 --

Warning: rewinddir(): supplied argument is not a valid Directory resource in %s on line %d
bool(false)

-- Iteration 15 --

Warning: rewinddir(): supplied argument is not a valid Directory resource in %s on line %d
bool(false)

-- Iteration 16 --

Warning: rewinddir(): supplied argument is not a valid Directory resource in %s on line %d
bool(false)

-- Iteration 17 --

Warning: rewinddir(): supplied argument is not a valid Directory resource in %s on line %d
bool(false)

-- Iteration 18 --

Warning: rewinddir(): supplied argument is not a valid Directory resource in %s on line %d
bool(false)

-- Iteration 19 --

Warning: rewinddir(): supplied argument is not a valid Directory resource in %s on line %d
bool(false)

-- Iteration 20 --

Warning: rewinddir(): supplied argument is not a valid Directory resource in %s on line %d
bool(false)

-- Iteration 21 --

Warning: rewinddir(): supplied argument is not a valid Directory resource in %s on line %d
bool(false)

-- Iteration 22 --

Warning: rewinddir(): supplied argument is not a valid Directory resource in %s on line %d
bool(false)

-- Iteration 23 --

Warning: rewinddir(): supplied argument is not a valid Directory resource in %s on line %d
bool(false)

-- Iteration 24 --

Warning: rewinddir(): supplied argument is not a valid Directory resource in %s on line %d
bool(false)
===DONE===
