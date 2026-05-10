--TEST--
Diamond + nullable intersection: implementer missing one branch is rejected
--FILE--
<?php
interface IFoo {}
interface IBar {}

interface Getter<out X : object> { public function get(): ?X; }
interface Flex extends Getter<IFoo>, Getter<IBar> {}

class Bad implements Flex {
    public function get(): ?IFoo { return null; }
}
?>
--EXPECTF--
Fatal error: Declaration of Bad::get(): ?IFoo must be compatible with Getter::get(): IFoo&IBar|null in %s on line %d
