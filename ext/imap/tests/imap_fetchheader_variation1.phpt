--TEST--
Test imap_fetchheader() function : usage variations - diff data types as $stream_id arg
--SKIPIF--
<?php
extension_loaded('imap') or die('skip imap extension not available in this build');
?>
--FILE--
<?php
/* Prototype  : string imap_fetchheader(resource $stream_id, int $msg_no [, int $options])
 * Description: Get the full unfiltered header for a message 
 * Source code: ext/imap/php_imap.c
 */

/*
 * Pass different data types as $stream_id argument to test behaviour of imap_fetchheader()
 */

echo "*** Testing imap_fetchheader() : usage variations ***\n";

// Initialise function arguments not being substituted
$msg_no = 1;

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

// get different types of array
$index_array = array (1, 2, 3);
$assoc_array = array ('one' => 1, 'two' => 2);

// get a resource variable
$fp = fopen(__FILE__, "r");

// unexpected values to be passed to $stream_id argument
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
       
       // array data
/*21*/ array(),
       $index_array,
       $assoc_array,
       array('foo', $index_array, $assoc_array),
       
       
       // object data
/*25*/ new classA(),

       // undefined data
/*26*/ @$undefined_var,

       // unset data
/*27*/ @$unset_var,
);

// loop through each element of $inputs to check the behavior of imap_fetchheader()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( imap_fetchheader($input, $msg_no) );
  $iterator++;
};
?>
===DONE===
--EXPECTF--
*** Testing imap_fetchheader() : usage variations ***

-- Iteration 1 --

Warning: imap_fetchheader(): supplied argument is not a valid imap resource in %s on line %d
bool(false)

-- Iteration 2 --

Warning: imap_fetchheader(): supplied argument is not a valid imap resource in %s on line %d
bool(false)

-- Iteration 3 --

Warning: imap_fetchheader(): supplied argument is not a valid imap resource in %s on line %d
bool(false)

-- Iteration 4 --

Warning: imap_fetchheader(): supplied argument is not a valid imap resource in %s on line %d
bool(false)

-- Iteration 5 --

Warning: imap_fetchheader(): supplied argument is not a valid imap resource in %s on line %d
bool(false)

-- Iteration 6 --

Warning: imap_fetchheader(): supplied argument is not a valid imap resource in %s on line %d
bool(false)

-- Iteration 7 --

Warning: imap_fetchheader(): supplied argument is not a valid imap resource in %s on line %d
bool(false)

-- Iteration 8 --

Warning: imap_fetchheader(): supplied argument is not a valid imap resource in %s on line %d
bool(false)

-- Iteration 9 --

Warning: imap_fetchheader(): supplied argument is not a valid imap resource in %s on line %d
bool(false)

-- Iteration 10 --

Warning: imap_fetchheader(): supplied argument is not a valid imap resource in %s on line %d
bool(false)

-- Iteration 11 --

Warning: imap_fetchheader(): supplied argument is not a valid imap resource in %s on line %d
bool(false)

-- Iteration 12 --

Warning: imap_fetchheader(): supplied argument is not a valid imap resource in %s on line %d
bool(false)

-- Iteration 13 --

Warning: imap_fetchheader(): supplied argument is not a valid imap resource in %s on line %d
bool(false)

-- Iteration 14 --

Warning: imap_fetchheader(): supplied argument is not a valid imap resource in %s on line %d
bool(false)

-- Iteration 15 --

Warning: imap_fetchheader(): supplied argument is not a valid imap resource in %s on line %d
bool(false)

-- Iteration 16 --

Warning: imap_fetchheader(): supplied argument is not a valid imap resource in %s on line %d
bool(false)

-- Iteration 17 --

Warning: imap_fetchheader(): supplied argument is not a valid imap resource in %s on line %d
bool(false)

-- Iteration 18 --

Warning: imap_fetchheader(): supplied argument is not a valid imap resource in %s on line %d
bool(false)

-- Iteration 19 --

Warning: imap_fetchheader(): supplied argument is not a valid imap resource in %s on line %d
bool(false)

-- Iteration 20 --

Warning: imap_fetchheader(): supplied argument is not a valid imap resource in %s on line %d
bool(false)

-- Iteration 21 --

Warning: imap_fetchheader(): supplied argument is not a valid imap resource in %s on line %d
bool(false)

-- Iteration 22 --

Warning: imap_fetchheader(): supplied argument is not a valid imap resource in %s on line %d
bool(false)

-- Iteration 23 --

Warning: imap_fetchheader(): supplied argument is not a valid imap resource in %s on line %d
bool(false)

-- Iteration 24 --

Warning: imap_fetchheader(): supplied argument is not a valid imap resource in %s on line %d
bool(false)

-- Iteration 25 --

Warning: imap_fetchheader(): supplied argument is not a valid imap resource in %s on line %d
bool(false)

-- Iteration 26 --

Warning: imap_fetchheader(): supplied argument is not a valid imap resource in %s on line %d
bool(false)

-- Iteration 27 --

Warning: imap_fetchheader(): supplied argument is not a valid imap resource in %s on line %d
bool(false)
===DONE===
