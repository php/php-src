--TEST--
GH-18301: casting to void is not allowed at the end of a for condition
--FILE--
<?php

for (;(void)true;);
?>
--EXPECTF--
Parse error: syntax error, unexpected token ";", expecting "," in %s on line %d
