--TEST--
Test imap_fetchbody() function : usage variation - diff data types as $section arg
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
 * Pass different data types as $section argument to test behaviour of imap_fetchbody()
 */

echo "*** Testing imap_fetchbody() : usage variations ***\n";

require_once(dirname(__FILE__).'/imap_include.inc');

// Initialise function arguments not being substituted
$stream_id = setup_test_mailbox('', 1); // set up temp mailbox with 1 simple msg
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

// unexpected values to be passed to $section argument
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
  var_dump( imap_fetchbody($stream_id, $msg_no, $input) );
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
.. mailbox '%s.phpttest' created

-- Iteration 1 --
string(71) "From: %s
To: %s
Subject: test1

"

-- Iteration 2 --
string(%d) "1: this is a test message, please ignore%a"

-- Iteration 3 --
string(0) ""

-- Iteration 4 --
string(0) ""

-- Iteration 5 --
string(0) ""

-- Iteration 6 --
string(0) ""

-- Iteration 7 --
string(0) ""

-- Iteration 8 --
string(0) ""

-- Iteration 9 --
string(0) ""

-- Iteration 10 --
string(%d) "From: %s
To: %s
Subject: test1

1: this is a test message, please ignore%a"

-- Iteration 11 --
string(%d) "From: %s
To: %s
Subject: test1

1: this is a test message, please ignore%a"

-- Iteration 12 --
string(%d) "1: this is a test message, please ignore%a"

-- Iteration 13 --
string(%d) "From: %s
To: %s
Subject: test1

1: this is a test message, please ignore%a"

-- Iteration 14 --
string(%d) "1: this is a test message, please ignore%a"

-- Iteration 15 --
string(%d) "From: %s
To: %s
Subject: test1

1: this is a test message, please ignore%a"

-- Iteration 16 --
string(%d) "From: %s
To: %s
Subject: test1

1: this is a test message, please ignore%a"

-- Iteration 17 --
string(%d) "From: %s
To: %s
Subject: test1

1: this is a test message, please ignore%a"

-- Iteration 18 --

Warning: imap_fetchbody() expects parameter 3 to be string, array given in %s on line 87
NULL

-- Iteration 19 --
string(0) ""

-- Iteration 20 --
string(0) ""

-- Iteration 21 --
string(0) ""

-- Iteration 22 --
string(0) ""

-- Iteration 23 --
string(%d) "From: %s
To: %s
Subject: test1

1: this is a test message, please ignore%a"

-- Iteration 24 --
string(%d) "From: %s
To: %s
Subject: test1

1: this is a test message, please ignore%a"
===DONE===
