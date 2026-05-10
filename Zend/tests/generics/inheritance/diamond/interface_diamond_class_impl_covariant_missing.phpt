--TEST--
Diamond + variance: class implementing a covariant interface diamond must return the intersection
--FILE--
<?php
interface IFoo {}
interface IBar {}
class JustFoo implements IFoo {}

interface Getter<+X : object> { public function get(): X; }
interface Flex extends Getter<IFoo>, Getter<IBar> {}

class Bad implements Flex {
    public function get(): JustFoo { return new JustFoo(); }
}
?>
--EXPECTF--
Fatal error: Declaration of Bad::get(): JustFoo must be compatible with %s::get(): IFoo&IBar in %s on line %d
