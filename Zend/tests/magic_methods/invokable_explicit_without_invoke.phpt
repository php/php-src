--TEST--
Explicit implements Invokable without __invoke() causes fatal error
--FILE--
<?php

class Bad implements Invokable {}

?>
--EXPECTF--
Fatal error: Class Bad must have an __invoke() method to implement Invokable in %s on line %d
