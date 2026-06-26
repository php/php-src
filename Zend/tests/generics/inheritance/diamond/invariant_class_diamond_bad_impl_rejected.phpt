--TEST--
Diamond + invariant T: bad concrete implementer is rejected by per-path LSP
--FILE--
<?php
interface IFoo {}
interface IBar {}

interface Tapper<T : object> {
    public function tap(T $object): T;
}

interface FooTapper extends Tapper<IFoo> {}
interface BarTapper extends Tapper<IBar> {}

class Bad implements FooTapper, BarTapper {
    public function tap(IFoo $object): IFoo { return $object; }
}
?>
--EXPECTF--
Fatal error: Declaration of Bad::tap(IFoo $object): IFoo must be compatible with Tapper::tap(IBar $object): IBar in %s on line %d
