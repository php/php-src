--TEST--
Test chmod() function : error conditions
--FILE--
<?php
echo "*** Testing chmod() : error conditions ***\n";

// testing chmod with a non-existing file
$filename = "___nonExisitingFile___";
var_dump(chmod($filename, 0777));

?>
--EXPECTF--
*** Testing chmod() : error conditions ***

Warning: chmod(): No such file or directory in %s on line %d
bool(false)
