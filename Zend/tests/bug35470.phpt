--TEST--
Bug #35470 (Assigning global using variable name from array doesn't function)
--FILE--
<?php
$x = array("test", "55");
global ${$x[0]};
${$x[0]} = $x[1];
echo "Test: $test\n";
?>
--EXPECT--
Test: 55
