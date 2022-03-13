--TEST--
Flexible nowdoc syntax error 8: no ending token with explicit trailing space
--FILE--
<?php

eval('<<<\'end\'
  ');

?>
--EXPECTF--
Parse error: syntax error, unexpected end of file in %s on line %d
