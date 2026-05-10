--TEST--
Diamond + invariant T: class implementing a use-site-merged interface diamond
--FILE--
<?php
interface IFoo {}
interface IBar {}
class FooBar implements IFoo, IBar {}

interface Tapper<T : object> {
    public function tap(T $object): T;
}

interface FlexTapper extends Tapper<IFoo>, Tapper<IBar> {}

class Concrete implements FlexTapper {
    public function tap(IFoo|IBar $o): IFoo&IBar {
        if ($o instanceof IFoo && $o instanceof IBar) {
            return $o;
        }
        return new FooBar();
    }
}

$c = new Concrete();
var_dump($c->tap(new FooBar()) instanceof FooBar);
var_dump($c instanceof FlexTapper, $c instanceof Tapper);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
