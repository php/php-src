--TEST--
Diamond + invariant T: Tapper<T> with T in both parameter and return positions, concrete class satisfies all paths via use-site variance
--FILE--
<?php

interface IFoo {}
interface IBar {}

class Foo implements IFoo {}
class Bar implements IBar {}

class FooBar implements IFoo, IBar {}

interface Tapper<T: object> {
    public function tap(T $object): T;
}

interface FooTapper extends Tapper<IFoo> {}
interface BarTapper extends Tapper<IBar> {}

class FooBarTapper implements FooTapper, BarTapper, Tapper<IFoo & IBar> {
    public function tap(IFoo | IBar $object): IFoo & IBar {
        if ($object instanceof IFoo) {
            if ($object instanceof IBar) {
                return $object;
            }

            return new FooBar();
        } else {
            return new FooBar();
        }
    }
}

$tapper = new FooBarTapper;
$a = $tapper->tap(new Foo());
$b = $tapper->tap(new Bar());
$c = $tapper->tap(new FooBar());

assert($a instanceof IFoo);
assert($a instanceof IBar);

assert($b instanceof IFoo);
assert($b instanceof IBar);

assert($c instanceof IFoo);
assert($c instanceof IBar);

echo "ok\n";
?>
--EXPECT--
ok
