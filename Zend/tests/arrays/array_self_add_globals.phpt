--TEST--
Add $GLOBALS to itself
--FILE--
<?php
$x = $GLOBALS + $GLOBALS;
?>
===DONE===
--EXPECT--
===DONE===
