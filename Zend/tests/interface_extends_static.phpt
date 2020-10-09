--TEST--
Interface cannot extend static, as it is reserved
--FILE--
<?php

interface Foo extends static {}

?>
--EXPECTF--
Fatal error: Cannot use 'static' as interface name, as it is reserved in %s on line %d
