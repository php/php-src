--TEST--
Required generic parameter after optional
--FILE--
<?php

class Test<T = int, T2> {}

?>
--EXPECTF--
Fatal error: Required generic parameter T2 follows optional in %s on line %d
