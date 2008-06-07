--TEST--
SPL: FastArray: Trying to instantiate passing string to construtor parameter
--FILE--
<?php

$a = new SplFastArray('FOO');

?>
--EXPECTF--
Warning: SplFastArray::__construct() expects parameter 1 to be long, string given in %s on line %d
