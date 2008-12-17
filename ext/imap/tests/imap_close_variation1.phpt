--TEST--
Test imap_close() function : usage variations - different data types as $stream_id arg
--SKIPIF--
<?php
extension_loaded('imap') or die('skip imap extension not available in this build');
?>
--FILE--
<?php
/* Prototype  : bool imap_close(resource $stream_id [, int $options])
 * Description: Close an IMAP stream 
 * Source code: ext/imap/php_imap.c
 */

/*
 * Pass different data types as $stream_id argument to test behaviour of imap_close()
 */

echo "*** Testing imap_close() : usage variations ***\n";

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

// loop through each element of $inputs to check the behavior of imap_close()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( imap_close($input) );
  $iterator++;
};
?>
===DONE===
--EXPECTF--
*** Testing imap_close() : usage variations ***

-- Iteration 1 --

Warning: imap_close() expects parameter 1 to be resource, integer given in %simap_close_variation1.php on line 80
NULL

-- Iteration 2 --

Warning: imap_close() expects parameter 1 to be resource, integer given in %simap_close_variation1.php on line 80
NULL

-- Iteration 3 --

Warning: imap_close() expects parameter 1 to be resource, integer given in %simap_close_variation1.php on line 80
NULL

-- Iteration 4 --

Warning: imap_close() expects parameter 1 to be resource, integer given in %simap_close_variation1.php on line 80
NULL

-- Iteration 5 --

Warning: imap_close() expects parameter 1 to be resource, double given in %simap_close_variation1.php on line 80
NULL

-- Iteration 6 --

Warning: imap_close() expects parameter 1 to be resource, double given in %simap_close_variation1.php on line 80
NULL

-- Iteration 7 --

Warning: imap_close() expects parameter 1 to be resource, double given in %simap_close_variation1.php on line 80
NULL

-- Iteration 8 --

Warning: imap_close() expects parameter 1 to be resource, double given in %simap_close_variation1.php on line 80
NULL

-- Iteration 9 --

Warning: imap_close() expects parameter 1 to be resource, double given in %simap_close_variation1.php on line 80
NULL

-- Iteration 10 --

Warning: imap_close() expects parameter 1 to be resource, null given in %simap_close_variation1.php on line 80
NULL

-- Iteration 11 --

Warning: imap_close() expects parameter 1 to be resource, null given in %simap_close_variation1.php on line 80
NULL

-- Iteration 12 --

Warning: imap_close() expects parameter 1 to be resource, boolean given in %simap_close_variation1.php on line 80
NULL

-- Iteration 13 --

Warning: imap_close() expects parameter 1 to be resource, boolean given in %simap_close_variation1.php on line 80
NULL

-- Iteration 14 --

Warning: imap_close() expects parameter 1 to be resource, boolean given in %simap_close_variation1.php on line 80
NULL

-- Iteration 15 --

Warning: imap_close() expects parameter 1 to be resource, boolean given in %simap_close_variation1.php on line 80
NULL

-- Iteration 16 --

Warning: imap_close() expects parameter 1 to be resource, %unicode_string_optional% given in %simap_close_variation1.php on line 80
NULL

-- Iteration 17 --

Warning: imap_close() expects parameter 1 to be resource, %unicode_string_optional% given in %simap_close_variation1.php on line 80
NULL

-- Iteration 18 --

Warning: imap_close() expects parameter 1 to be resource, array given in %simap_close_variation1.php on line 80
NULL

-- Iteration 19 --

Warning: imap_close() expects parameter 1 to be resource, %unicode_string_optional% given in %simap_close_variation1.php on line 80
NULL

-- Iteration 20 --

Warning: imap_close() expects parameter 1 to be resource, %unicode_string_optional% given in %simap_close_variation1.php on line 80
NULL

-- Iteration 21 --

Warning: imap_close() expects parameter 1 to be resource, %unicode_string_optional% given in %simap_close_variation1.php on line 80
NULL

-- Iteration 22 --

Warning: imap_close() expects parameter 1 to be resource, object given in %simap_close_variation1.php on line 80
NULL

-- Iteration 23 --

Warning: imap_close() expects parameter 1 to be resource, null given in %simap_close_variation1.php on line 80
NULL

-- Iteration 24 --

Warning: imap_close() expects parameter 1 to be resource, null given in %simap_close_variation1.php on line 80
NULL
===DONE===
