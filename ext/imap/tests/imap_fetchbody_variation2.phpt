--TEST--
Test imap_fetchbody() function : usage variation - diff data types as $msg_no arg
--SKIPIF--
<?php
require_once(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php
/* Prototype  : string imap_fetchbody(resource $stream_id, int $msg_no, string $section
 *           [, int $options])
 * Description: Get a specific body section
 * Source code: ext/imap/php_imap.c
 */

/*
 * Pass different data types as $msg_no argument to test behaviour of imap_fetchbody()
 */

echo "*** Testing imap_fetchbody() : usage variations ***\n";

require_once(dirname(__FILE__).'/imap_include.inc');

// Initialise function arguments not being substituted
$stream_id = setup_test_mailbox('', 1); // set up temp mailbox with 1 simple msg
$section = '1';

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

// unexpected values to be passed to $msg_no argument
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
  var_dump( imap_fetchbody($stream_id, $input, $section) );
  $iterator++;
};
?>
===DONE===
--CLEAN--
<?php
require_once(dirname(__FILE__).'/clean.inc');
?>
--EXPECTF--
*** Testing imap_fetchbody() : usage variations ***
Create a temporary mailbox and add 1 msgs
.. mailbox '{%s}%s' created

-- Iteration 1 --

Warning: imap_fetchbody(): Bad message number in %s on line %d
bool(false)

-- Iteration 2 --
string(%d) "1: this is a test message, please ignore%a"

-- Iteration 3 --

Warning: imap_fetchbody(): Bad message number in %s on line %d
bool(false)

-- Iteration 4 --

Warning: imap_fetchbody(): Bad message number in %s on line %d
bool(false)

-- Iteration 5 --

Warning: imap_fetchbody(): Bad message number in %s on line %d
bool(false)

-- Iteration 6 --

Warning: imap_fetchbody(): Bad message number in %s on line %d
bool(false)

-- Iteration 7 --

Warning: imap_fetchbody(): Bad message number in %s on line %d
bool(false)

-- Iteration 8 --

Warning: imap_fetchbody(): Bad message number in %s on line %d
bool(false)

-- Iteration 9 --

Warning: imap_fetchbody(): Bad message number in %s on line %d
bool(false)

-- Iteration 10 --

Warning: imap_fetchbody(): Bad message number in %s on line %d
bool(false)

-- Iteration 11 --

Warning: imap_fetchbody(): Bad message number in %s on line %d
bool(false)

-- Iteration 12 --
string(%d) "1: this is a test message, please ignore%a"

-- Iteration 13 --

Warning: imap_fetchbody(): Bad message number in %s on line %d
bool(false)

-- Iteration 14 --
string(%d) "1: this is a test message, please ignore%a"

-- Iteration 15 --

Warning: imap_fetchbody(): Bad message number in %s on line %d
bool(false)

-- Iteration 16 --

Warning: imap_fetchbody() expects parameter 2 to be integer, string given in %s on line %d
NULL

-- Iteration 17 --

Warning: imap_fetchbody() expects parameter 2 to be integer, string given in %s on line %d
NULL

-- Iteration 18 --

Warning: imap_fetchbody() expects parameter 2 to be integer, array given in %s on line %d
NULL

-- Iteration 19 --

Warning: imap_fetchbody() expects parameter 2 to be integer, string given in %s on line %d
NULL

-- Iteration 20 --

Warning: imap_fetchbody() expects parameter 2 to be integer, string given in %s on line %d
NULL

-- Iteration 21 --

Warning: imap_fetchbody() expects parameter 2 to be integer, string given in %s on line %d
NULL

-- Iteration 22 --

Warning: imap_fetchbody() expects parameter 2 to be integer, object given in %s on line %d
NULL

-- Iteration 23 --

Warning: imap_fetchbody(): Bad message number in %s on line %d
bool(false)

-- Iteration 24 --

Warning: imap_fetchbody(): Bad message number in %s on line %d
bool(false)
===DONE===
