--TEST--
Test is_readable() function: error conditions
--FILE--
<?php
echo "\n*** Testing is_readable() on non-existent file ***\n";
var_dump( is_readable(__DIR__."/is_readable_error.tmp") );

echo "Done\n";
?>
--EXPECT--
*** Testing is_readable() on non-existent file ***
bool(false)
Done
