--TEST--
Add $GLOBALS to itself
--FILE--
<?php
$GLOBALS += $GLOBALS;
$x = $GLOBALS + $GLOBALS;
?>
===DONE===
--EXPECT--
===DONE===
