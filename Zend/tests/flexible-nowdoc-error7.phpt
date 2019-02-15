--TEST--
Flexible nowdoc syntax error 7: no ending token
--DESCRIPTION--
Note: the closing ?> has been deliberately elided.
--FILE--
<?php

echo <<<'END'

--EXPECTF--
Parse error: syntax error, unexpected end of file, expecting variable (T_VARIABLE) or heredoc end (T_END_HEREDOC) or ${ (T_DOLLAR_OPEN_CURLY_BRACES) or {$ (T_CURLY_OPEN) in %s on line %d
