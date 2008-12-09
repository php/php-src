--TEST--
Test session_unregister() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/* 
 * Prototype : bool session_is_registered(string $name)
 * Description : Find out whether a global variable is registered in a session
 * Source code : ext/session/session.c 
 */

echo "*** Testing session_is_registered() : variation ***\n";

// Get an unset variable
$unset_var = 10;
unset($unset_var);

class classA
{
    public function __toString() {
        return "Hello World!";
    }
}

$heredoc = <<<EOT
Hello World!
EOT;

$fp = fopen(__FILE__, "r");

// Unexpected values to be passed as arguments
$inputs = array(

       // Integer data
/*1*/  0,
       1,
       12345,
       -2345,

       // Float data
/*5*/  10.5,
       -10.5,
       12.3456789000e10,
       12.3456789000E-10,
       .5,

       // Null data
/*10*/ NULL,
       null,

       // Boolean data
/*12*/ true,
       false,
       TRUE,
       FALSE,
       
       // Empty strings
/*16*/ "",
       '',

       // Invalid string data
/*18*/ "Nothing",
       'Nothing',
       $heredoc,
       
       // Object data
/*21*/ new classA(),

       // Undefined data
/*22*/ @$undefined_var,

       // Unset data
/*23*/ @$unset_var,

       // Resource variable
/*24*/ $fp
);

var_dump(session_start());
var_dump($_SESSION);
var_dump(session_is_registered($inputs));
var_dump($_SESSION);
var_dump(session_destroy());
var_dump($_SESSION);

fclose($fp);
echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_is_registered() : variation ***
bool(true)
array(0) {
}

Deprecated: Function session_is_registered() is deprecated in %s on line %d

Warning: session_is_registered() expects parameter 1 to be string, array given in %s on line %d
NULL
array(0) {
}
bool(true)
array(0) {
}
Done
