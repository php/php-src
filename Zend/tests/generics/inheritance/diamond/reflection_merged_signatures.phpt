--TEST--
Reflection: merged diamond signatures are visible through ReflectionMethod / ReflectionType
--FILE--
<?php
interface IFoo {}
interface IBar {}
class FooBar implements IFoo, IBar {}

interface Tapper<T : object> {
    public function tap(T $o): T;
}

interface FlexTapper extends Tapper<IFoo>, Tapper<IBar> {}

class C implements FlexTapper {
    public function tap(IFoo|IBar $o): IFoo&IBar {
        if ($o instanceof IFoo && $o instanceof IBar) {
            return $o;
        }
        return new FooBar();
    }
}

$flex = (new ReflectionClass(FlexTapper::class))->getMethod('tap');
echo "FlexTapper::tap return = ", (string)$flex->getReturnType(), "\n";
echo "FlexTapper::tap param  = ", (string)$flex->getParameters()[0]->getType(), "\n";
echo "FlexTapper::tap return is intersection: ",
    ($flex->getReturnType() instanceof ReflectionIntersectionType ? "yes" : "no"), "\n";
echo "FlexTapper::tap param  is union:        ",
    ($flex->getParameters()[0]->getType() instanceof ReflectionUnionType ? "yes" : "no"), "\n";

$c = (new ReflectionClass(C::class))->getMethod('tap');
echo "C::tap return = ", (string)$c->getReturnType(), "\n";
echo "C::tap param  = ", (string)$c->getParameters()[0]->getType(), "\n";
echo "C::tap prototype declaring class = ", $c->getPrototype()->getDeclaringClass()->getName(), "\n";

$base = (new ReflectionClass(Tapper::class))->getMethod('tap');
echo "Tapper::tap return = ", (string)$base->getReturnType(), "\n";
echo "Tapper::tap param  = ", (string)$base->getParameters()[0]->getType(), "\n";
?>
--EXPECT--
FlexTapper::tap return = IFoo&IBar
FlexTapper::tap param  = IFoo|IBar
FlexTapper::tap return is intersection: yes
FlexTapper::tap param  is union:        yes
C::tap return = IFoo&IBar
C::tap param  = IFoo|IBar
C::tap prototype declaring class = Tapper
Tapper::tap return = object
Tapper::tap param  = object
