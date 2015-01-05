--TEST--
SPL: FixedArray: Trying to instantiate passing string to construtor parameter
--FILE--
<?php

$a = new SplFixedArray('FOO');

?>
--EXPECTF--
Warning: SplFixedArray::__construct() expects parameter 1 to be integer, string given in %s on line %d
