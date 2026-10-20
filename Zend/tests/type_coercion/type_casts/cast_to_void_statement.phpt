--TEST--
casting to void is a statement
--FILE--
<?php

$tmp = (void)$dummy;

echo "Done\n";
?>
--EXPECTF--
Parse error: syntax error, unexpected token "(void)" in %s on line %d
