--TEST--
#[NotSerializable] traits behavior
--FILE--
<?php

#[NotSerializable]
trait Foo {

}

?>
--EXPECTF--
Fatal error: Cannot apply #[\NotSerializable] to trait Foo in %s on line %d
