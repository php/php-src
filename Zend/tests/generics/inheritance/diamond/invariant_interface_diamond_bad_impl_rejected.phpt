--TEST--
Diamond + invariant T: class missing a path of the use-site-merged interface contract is rejected
--FILE--
<?php
interface IFoo {}
interface IBar {}

interface Tapper<T : object> {
    public function tap(T $object): T;
}

interface FlexTapper extends Tapper<IFoo>, Tapper<IBar> {}

class Bad implements FlexTapper {
    public function tap(IFoo $o): IFoo { return $o; }
}
?>
--EXPECTF--
Fatal error: Declaration of Bad::tap(IFoo $o): IFoo must be compatible with %s::tap(IFoo|IBar $object): IFoo&IBar in %s on line %d
