--TEST--
Test is_writable() and its alias is_writeable() function: error conditions
--FILE--
<?php
echo "\n*** Testing is_writable() on non-existent file ***\n";
var_dump( is_writable(__DIR__."/is_writable") );
var_dump( is_writeable(__DIR__."/is_writable") );

echo "Done\n";
?>
--EXPECT--
*** Testing is_writable() on non-existent file ***
bool(false)
bool(false)
Done
