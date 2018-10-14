--TEST--
Test imap_fetch_overview() function : usage variations - diff data types as $msg_no arg
--SKIPIF--
<?php
require_once(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php
/* Prototype  : array imap_fetch_overview(resource $stream_id, int $msg_no [, int $options])
 * Description: Read an overview of the information in the headers
 * of the given message sequence
 * Source code: ext/imap/php_imap.c
 */

/*
 * Pass different data types as $msg_no argument to imap_fetch_overview() to test behaviour
 */

echo "*** Testing imap_fetch_overview() : usage variations ***\n";
require_once(dirname(__FILE__).'/imap_include.inc');

// Initialise function arguments not being substituted
$stream_id = setup_test_mailbox('', 1, $mailbox, 'notSimple'); // set up temp mailbox with 1 msg

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

// unexpected values to be passed to <<<ARGUMENT HERE>>> argument
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

// loop through each element of $inputs to check the behavior of imap_fetch_overview()
$iterator = 1;
foreach($inputs as $input) {
	echo "\n-- Testing with second argument value: ";
	var_dump($input);
	$overview = imap_fetch_overview($stream_id, $input);
	if (!$overview) {
		echo imap_last_error() . "\n";
	} else {
		displayOverviewFields($overview[0]);
        }
	$iterator++;
};

fclose($fp);

// clear the error stack
imap_errors();
?>
===DONE===
--CLEAN--
<?php
require_once(dirname(__FILE__).'/clean.inc');
?>
--EXPECTF--
*** Testing imap_fetch_overview() : usage variations ***
Create a temporary mailbox and add 1 msgs
.. mailbox '{%s}%s' created

-- Testing with second argument value: int(0)
Sequence out of range

-- Testing with second argument value: int(1)
size is %d
uid is %d
msgno is 1
recent is %d
flagged is 0
answered is 0
deleted is 0
seen is 0
draft is 0
udate is OK

-- Testing with second argument value: int(12345)
Sequence out of range

-- Testing with second argument value: int(-2345)
Syntax error in sequence

-- Testing with second argument value: float(10.5)
Sequence out of range

-- Testing with second argument value: float(-10.5)
Syntax error in sequence

-- Testing with second argument value: float(123456789000)
Sequence out of range

-- Testing with second argument value: float(1.23456789E-9)
Sequence syntax error

-- Testing with second argument value: float(0.5)
Sequence out of range

-- Testing with second argument value: NULL
Sequence out of range

-- Testing with second argument value: NULL
Sequence out of range

-- Testing with second argument value: bool(true)
size is %d
uid is %d
msgno is 1
recent is %d
flagged is 0
answered is 0
deleted is 0
seen is 0
draft is 0
udate is OK

-- Testing with second argument value: bool(false)
Sequence out of range

-- Testing with second argument value: bool(true)
size is %d
uid is %d
msgno is 1
recent is %d
flagged is 0
answered is 0
deleted is 0
seen is 0
draft is 0
udate is OK

-- Testing with second argument value: bool(false)
Sequence out of range

-- Testing with second argument value: string(0) ""
Sequence out of range

-- Testing with second argument value: string(0) ""
Sequence out of range

-- Testing with second argument value: array(0) {
}

Warning: imap_fetch_overview() expects parameter 2 to be string, array given in %s on line %d
Sequence out of range

-- Testing with second argument value: string(6) "string"
Syntax error in sequence

-- Testing with second argument value: string(6) "string"
Syntax error in sequence

-- Testing with second argument value: string(11) "hello world"
Syntax error in sequence

-- Testing with second argument value: object(classA)#1 (0) {
}
Syntax error in sequence

-- Testing with second argument value: NULL
Syntax error in sequence

-- Testing with second argument value: NULL
Syntax error in sequence

-- Testing with second argument value: resource(%d) of type (stream)

Warning: imap_fetch_overview() expects parameter 2 to be string, resource given in %s on line %d
Syntax error in sequence
===DONE===
