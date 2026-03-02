--TEST--
Test mkdir() and rmdir() functions : error conditions
--FILE--
<?php

echo "\n*** Testing rmdir() on non-existent directory ***\n";
var_dump( rmdir("temp") );

echo "Done\n";
?>
--EXPECTF--
*** Testing rmdir() on non-existent directory ***

Warning: rmdir(temp): No such file or directory in %s on line %d
bool(false)
Done
