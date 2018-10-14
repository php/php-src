--TEST--
Test imap_fetchbody() function : usage variation - diff data types as $stream_id arg
--SKIPIF--
<?php
extension_loaded('imap') or die('skip imap extension not available in this build');
?>
--FILE--
<?php
/* Prototype  : string imap_fetchbody(resource $stream_id, int $msg_no, string $section
 *           [, int $options])
 * Description: Get a specific body section
 * Source code: ext/imap/php_imap.c
 */

/*
 * Pass different data types as $stream_id argument to test behaviour of imap_fetchbody()
 */

echo "*** Testing imap_fetchbody() : usage variations ***\n";

// Initialise function arguments not being substituted
$msg_no = 1;
$section = '2';

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

// loop through each element of $inputs to check the behavior of imap_fetchbody()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( imap_fetchbody($input, $msg_no, $section) );
  $iterator++;
}
?>
===DONE===
--EXPECTF--
*** Testing imap_fetchbody() : usage variations ***

-- Iteration 1 --

Warning: imap_fetchbody() expects parameter 1 to be resource, integer given in %s on line 85
NULL

-- Iteration 2 --

Warning: imap_fetchbody() expects parameter 1 to be resource, integer given in %s on line 85
NULL

-- Iteration 3 --

Warning: imap_fetchbody() expects parameter 1 to be resource, integer given in %s on line 85
NULL

-- Iteration 4 --

Warning: imap_fetchbody() expects parameter 1 to be resource, integer given in %s on line 85
NULL

-- Iteration 5 --

Warning: imap_fetchbody() expects parameter 1 to be resource, float given in %s on line 85
NULL

-- Iteration 6 --

Warning: imap_fetchbody() expects parameter 1 to be resource, float given in %s on line 85
NULL

-- Iteration 7 --

Warning: imap_fetchbody() expects parameter 1 to be resource, float given in %s on line 85
NULL

-- Iteration 8 --

Warning: imap_fetchbody() expects parameter 1 to be resource, float given in %s on line 85
NULL

-- Iteration 9 --

Warning: imap_fetchbody() expects parameter 1 to be resource, float given in %s on line 85
NULL

-- Iteration 10 --

Warning: imap_fetchbody() expects parameter 1 to be resource, null given in %s on line 85
NULL

-- Iteration 11 --

Warning: imap_fetchbody() expects parameter 1 to be resource, null given in %s on line 85
NULL

-- Iteration 12 --

Warning: imap_fetchbody() expects parameter 1 to be resource, boolean given in %s on line 85
NULL

-- Iteration 13 --

Warning: imap_fetchbody() expects parameter 1 to be resource, boolean given in %s on line 85
NULL

-- Iteration 14 --

Warning: imap_fetchbody() expects parameter 1 to be resource, boolean given in %s on line 85
NULL

-- Iteration 15 --

Warning: imap_fetchbody() expects parameter 1 to be resource, boolean given in %s on line 85
NULL

-- Iteration 16 --

Warning: imap_fetchbody() expects parameter 1 to be resource, string given in %s on line 85
NULL

-- Iteration 17 --

Warning: imap_fetchbody() expects parameter 1 to be resource, string given in %s on line 85
NULL

-- Iteration 18 --

Warning: imap_fetchbody() expects parameter 1 to be resource, array given in %s on line 85
NULL

-- Iteration 19 --

Warning: imap_fetchbody() expects parameter 1 to be resource, string given in %s on line 85
NULL

-- Iteration 20 --

Warning: imap_fetchbody() expects parameter 1 to be resource, string given in %s on line 85
NULL

-- Iteration 21 --

Warning: imap_fetchbody() expects parameter 1 to be resource, string given in %s on line 85
NULL

-- Iteration 22 --

Warning: imap_fetchbody() expects parameter 1 to be resource, object given in %s on line 85
NULL

-- Iteration 23 --

Warning: imap_fetchbody() expects parameter 1 to be resource, null given in %s on line 85
NULL

-- Iteration 24 --

Warning: imap_fetchbody() expects parameter 1 to be resource, null given in %s on line 85
NULL
===DONE===
