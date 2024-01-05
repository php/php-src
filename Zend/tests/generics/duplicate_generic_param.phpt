--TEST--
Duplicate generic parameter name
--FILE--
<?php

class Test<T, T> {
}

?>
--EXPECTF--
Fatal error: Duplicate generic parameter T in %s on line %d
