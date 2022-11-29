--TEST--
Aliasing built-in types
--FILE--
<?php

use bool as A;

?>
--EXPECTF--
Fatal error: Cannot alias 'bool' as it is a built-in type in %s on line %d
