--TEST--
Test get_extension_funcs() function: basic test
--FILE--
<?php
echo "Simple testcase for get_extension_funcs() function\n";

$result = get_extension_funcs("standard");
var_dump(gettype($result));
var_dump(in_array("cos", $result));

// Unknown extension
var_dump(get_extension_funcs("foo"));

?>
--EXPECT--
Simple testcase for get_extension_funcs() function
string(5) "array"
bool(true)
bool(false)
