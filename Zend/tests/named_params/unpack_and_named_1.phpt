--TEST--
Mixing unpacking and named params (1)
--FILE--
<?php

test(...[], a: 42);

?>
--EXPECTF--
Fatal error: Cannot combine named arguments and argument unpacking in %s on line %d
