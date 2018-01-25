--TEST--
Testing array with '[]' passed as argument by reference
--FILE--
<?php

function test(&$var) { }
test($arr[]);

print "ok!\n";

?>
--EXPECT--
ok!
