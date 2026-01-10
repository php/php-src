--TEST--
Attempting to define a goto label called exit and jump to it
--FILE--
<?php

echo "Before\n";
goto exit;
echo "In between\n";
exit:
echo "After\n";

?>
--EXPECTF--
Parse error: syntax error, unexpected token "exit", expecting identifier in %s on line %d
