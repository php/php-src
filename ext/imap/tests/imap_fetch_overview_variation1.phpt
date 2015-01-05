--TEST--
Test imap_fetch_overview() function : usage variations - diff data types as $stream_id arg
--SKIPIF--
<?php
extension_loaded('imap') or die('skip imap extension not available in this build');
?>
--FILE--
<?php
/* Prototype  : array imap_fetch_overview(resource $stream_id, int $msg_no [, int $options])
 * Description: Read an overview of the information in the headers 
 * of the given message sequence 
 * Source code: ext/imap/php_imap.c
 */

/*
 * Pass different data types as $stream_id argument to imap_fetch_overview() to test behaviour
 */

echo "*** Testing imap_fetch_overview() : usage variations ***\n";

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

// loop through each element of $inputs to check the behavior of imap_fetch_overview()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Testing with first argument value: ";
  var_dump($input);
  var_dump( imap_fetch_overview($input, $msg_no) );
  $iterator++;
};
?>
===DONE===
--EXPECTF--
*** Testing imap_fetch_overview() : usage variations ***

-- Testing with first argument value: int(0)

Warning: imap_fetch_overview() expects parameter 1 to be resource, integer given in %s on line %d
NULL

-- Testing with first argument value: int(1)

Warning: imap_fetch_overview() expects parameter 1 to be resource, integer given in %s on line %d
NULL

-- Testing with first argument value: int(12345)

Warning: imap_fetch_overview() expects parameter 1 to be resource, integer given in %s on line %d
NULL

-- Testing with first argument value: int(-2345)

Warning: imap_fetch_overview() expects parameter 1 to be resource, integer given in %s on line %d
NULL

-- Testing with first argument value: float(10.5)

Warning: imap_fetch_overview() expects parameter 1 to be resource, float given in %s on line %d
NULL

-- Testing with first argument value: float(-10.5)

Warning: imap_fetch_overview() expects parameter 1 to be resource, float given in %s on line %d
NULL

-- Testing with first argument value: float(123456789000)

Warning: imap_fetch_overview() expects parameter 1 to be resource, float given in %s on line %d
NULL

-- Testing with first argument value: float(1.23456789E-9)

Warning: imap_fetch_overview() expects parameter 1 to be resource, float given in %s on line %d
NULL

-- Testing with first argument value: float(0.5)

Warning: imap_fetch_overview() expects parameter 1 to be resource, float given in %s on line %d
NULL

-- Testing with first argument value: NULL

Warning: imap_fetch_overview() expects parameter 1 to be resource, null given in %s on line %d
NULL

-- Testing with first argument value: NULL

Warning: imap_fetch_overview() expects parameter 1 to be resource, null given in %s on line %d
NULL

-- Testing with first argument value: bool(true)

Warning: imap_fetch_overview() expects parameter 1 to be resource, boolean given in %s on line %d
NULL

-- Testing with first argument value: bool(false)

Warning: imap_fetch_overview() expects parameter 1 to be resource, boolean given in %s on line %d
NULL

-- Testing with first argument value: bool(true)

Warning: imap_fetch_overview() expects parameter 1 to be resource, boolean given in %s on line %d
NULL

-- Testing with first argument value: bool(false)

Warning: imap_fetch_overview() expects parameter 1 to be resource, boolean given in %s on line %d
NULL

-- Testing with first argument value: %string|unicode%(0) ""

Warning: imap_fetch_overview() expects parameter 1 to be resource, %unicode_string_optional% given in %s on line %d
NULL

-- Testing with first argument value: %string|unicode%(0) ""

Warning: imap_fetch_overview() expects parameter 1 to be resource, %unicode_string_optional% given in %s on line %d
NULL

-- Testing with first argument value: array(0) {
}

Warning: imap_fetch_overview() expects parameter 1 to be resource, array given in %s on line %d
NULL

-- Testing with first argument value: %string|unicode%(6) "string"

Warning: imap_fetch_overview() expects parameter 1 to be resource, %unicode_string_optional% given in %s on line %d
NULL

-- Testing with first argument value: %string|unicode%(6) "string"

Warning: imap_fetch_overview() expects parameter 1 to be resource, %unicode_string_optional% given in %s on line %d
NULL

-- Testing with first argument value: %string|unicode%(11) "hello world"

Warning: imap_fetch_overview() expects parameter 1 to be resource, %unicode_string_optional% given in %s on line %d
NULL

-- Testing with first argument value: object(classA)#1 (0) {
}

Warning: imap_fetch_overview() expects parameter 1 to be resource, object given in %s on line %d
NULL

-- Testing with first argument value: NULL

Warning: imap_fetch_overview() expects parameter 1 to be resource, null given in %s on line %d
NULL

-- Testing with first argument value: NULL

Warning: imap_fetch_overview() expects parameter 1 to be resource, null given in %s on line %d
NULL
===DONE===
