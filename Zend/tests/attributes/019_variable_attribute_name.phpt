--TEST--
Attribute name cannot be a variable
--FILE--
<?php

#[$x]
class A {}

?>
--EXPECTF--
Parse error: syntax error, unexpected variable "$x" in %s on line %d
