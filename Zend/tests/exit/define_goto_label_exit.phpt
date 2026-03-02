--TEST--
Attempting to define a goto label called exit
--FILE--
<?php

echo "Before\n";

echo "In between\n";
exit:
echo "After\n";

?>
--EXPECTF--
Parse error: syntax error, unexpected token ":" in %s on line %d
