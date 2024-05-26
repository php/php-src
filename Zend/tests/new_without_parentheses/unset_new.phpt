--TEST--
Cannot unset new expression
--FILE--
<?php

unset(new ArrayObject());

?>
--EXPECTF--
Parse error: syntax error, unexpected token ")", expecting "->" or "?->" or "{" or "[" in %s on line %d
