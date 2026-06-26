--TEST--
Diamond + property hook (get-only): impl missing one branch of the merged intersection is rejected
--FILE--
<?php
interface HasX<out T : object> {
    public T $x { get; }
}
interface IFoo {}
interface IBar {}

interface FlexX extends HasX<IFoo>, HasX<IBar> {}

class Bad implements FlexX {
    public IFoo $x { get => new class implements IFoo {}; }
}
?>
--EXPECTF--
Fatal error: Declaration of Bad::$x::get(): IFoo must be compatible with HasX::$x::get(): IFoo&IBar in %s on line %d
