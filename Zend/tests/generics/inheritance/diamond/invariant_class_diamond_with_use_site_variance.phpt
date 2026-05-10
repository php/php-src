--TEST--
Diamond + invariant T: a concrete class can satisfy multiple bindings via use-site variance (param union, return intersection)
--FILE--
<?php
interface IFoo {}
interface IBar {}

final class FooBar implements IFoo, IBar {}

interface Tapper<T : object> {
    public function tap(T $object): T;
}

interface FooTapper extends Tapper<IFoo> {}
interface BarTapper extends Tapper<IBar> {}

final class FooBarTapper implements FooTapper, BarTapper, Tapper<IFoo & IBar> {
    public function tap(IFoo | IBar $object): IFoo & IBar {
        if ($object instanceof IFoo && $object instanceof IBar) {
            return $object;
        }
        return new FooBar();
    }
}

$t = new FooBarTapper();
var_dump($t->tap(new FooBar()) instanceof FooBar);
var_dump($t instanceof FooTapper, $t instanceof BarTapper, $t instanceof Tapper);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
