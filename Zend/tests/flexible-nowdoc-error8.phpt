--TEST--
Flexible nowdoc syntax error 8: no ending token with explicit trailing space
--FILE--
<?php

eval('<<<\'end\'
  ');

?>
--EXPECTF--
Parse error: syntax error, unexpected end of file, expecting variable (T_VARIABLE) or heredoc end (T_END_HEREDOC) or ${ (T_DOLLAR_OPEN_CURLY_BRACES) or {$ (T_CURLY_OPEN) in %s on line %d
