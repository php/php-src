--TEST--
#[NotSerializable] interface behavior
--FILE--
<?php

#[NotSerializable]
interface Foo {

}

?>
--EXPECTF--
Fatal error: Cannot apply #[\NotSerializable] to interface Foo in %s on line %d
