--TEST--
Interface cannot extend static, as it is reserved
--FILE--
<?php

interface Foo extends static {}

?>
--EXPECTF--
Fatal error: Cannot use "static" as interface name, static is a reserved class name in %s on line %d
