--TEST--
Flexible nowdoc syntax error 6: no ending token on 0 length body
--DESCRIPTION--
Note: the closing ?> has been deliberately elided.
--FILE--
<?php

echo <<<'END'
?>
--EXPECTF--
Parse error: syntax error, unexpected end of file in %s on line %d
