--TEST--
Test get_include_path() function
--INI--
include_path=.
--FILE--
<?php


echo "*** Testing get_include_path()\n";

var_dump(get_include_path());

if (ini_get("include_path") == get_include_path()) {
    echo "PASSED\n";
} else {
    echo "FAILED\n";
}

?>
--EXPECT--
*** Testing get_include_path()
string(1) "."
PASSED
