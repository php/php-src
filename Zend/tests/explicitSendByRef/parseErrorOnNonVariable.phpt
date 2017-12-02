--TEST--
Using explicit pass-by-ref with a non-variable results in a parse error
--FILE--
<?php

function byRef(&$a) {}
byRef(&null);

?>
--EXPECTF--
Parse error: syntax error, unexpected ')', expecting %s in %s on line %d
