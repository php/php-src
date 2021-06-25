--TEST--
Test file_exists() function : usage variations
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
echo "*** Testing file_exists() : usage variations ***\n";

var_dump(file_exists(false));
var_dump(file_exists(''));
var_dump(file_exists(' '));
var_dump(file_exists('|'));
echo "Done";
?>
--EXPECTF--
*** Testing file_exists() : usage variations ***

Deprecated: Implicit bool to string coercion is deprecated in %s on line %d
bool(false)
bool(false)
bool(false)
bool(false)
Done
