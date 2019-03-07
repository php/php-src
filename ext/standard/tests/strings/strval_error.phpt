--TEST--
Test strval() function : usage variations  - error conditions
--FILE--
<?php
/* Prototype  : string strval  ( mixed $var  )
 * Description: Get the string value of a variable.
 * Source code: ext/standard/string.c
 */

echo "*** Testing strval() : error conditions ***\n";

error_reporting(E_ALL ^ E_NOTICE);

class MyClass
{
	// no toString() method defined
}

// Testing strval with a object which has no toString() method
echo "\n-- Testing strval() function with object which has not toString() method  --\n";
var_dump( strval(new MyClass()) );

?>
===DONE===
--EXPECTF--
*** Testing strval() : error conditions ***

-- Testing strval() function with object which has not toString() method  --

Recoverable fatal error: Object of class MyClass could not be converted to string in %s on line %d
