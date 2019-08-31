--TEST--
Test get_include_path() function
--INI--
include_path=.
--FILE--
<?php
/* Prototype: string get_include_path  ( void  )
 * Description: Gets the current include_path configuration option

*/

echo "*** Testing get_include_path()\n";

var_dump(get_include_path());

if (ini_get("include_path") == get_include_path()) {
	echo "PASSED\n";
} else {
	echo "FAILED\n";
}

?>
===DONE===
--EXPECTF--
*** Testing get_include_path()
string(1) "."
PASSED
===DONE===
