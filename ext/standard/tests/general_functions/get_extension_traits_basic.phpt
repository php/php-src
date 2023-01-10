--TEST--
Test get_extension_traits() function: basic test
--FILE--
<?php
echo "Simple testcase for get_extension_traits() function\n";

$result = get_extension_traits("standard");
var_dump(gettype($result));

// Unknown extension
var_dump(get_extension_traits("foo"));

?>
--EXPECT--
Simple testcase for get_extension_traits() function
string(5) "array"
bool(false)
