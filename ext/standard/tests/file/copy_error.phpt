--TEST--
Test copy() function: error conditions
--FILE--
<?php
echo "*** Testing copy() function: error conditions --\n";
/* Invalid args */
var_dump( copy("/no/file", "file") );

echo "*** Done ***\n";
?>
--EXPECTF--
*** Testing copy() function: error conditions --

Warning: copy(/no/file): Failed to open stream: No such file or directory in %s on line %d
bool(false)
*** Done ***
