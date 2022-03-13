--TEST--
Flexible heredoc syntax error 7: no ending token
--DESCRIPTION--
Note: the closing ?> has been deliberately elided.
--FILE--
<?php

echo <<<END

--EXPECTF--
Parse error: syntax error, unexpected end of file in %s on line %d
