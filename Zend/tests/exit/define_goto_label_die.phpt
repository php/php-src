--TEST--
Attempting to define a goto label called die
--FILE--
<?php

echo "Before\n";

echo "In between\n";
die:
echo "After\n";

?>
--EXPECTF--
Parse error: syntax error, unexpected token ":" in %s on line %d
