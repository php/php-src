--TEST--
Diamond + trait-use: covariant return merges as intersection
--FILE--
<?php
interface IFoo {}
interface IBar {}
class FooBar implements IFoo, IBar {}

trait HasGetter<+X : object> {
    public function get(): X { return new FooBar(); }
}

class C {
    use HasGetter<IFoo>, HasGetter<IBar>;
}

$r = new ReflectionClass(C::class);
$type = $r->getMethod('get')->getReturnType();
echo "return: ", (string)$type, "\n";
echo "is intersection: ", ($type instanceof ReflectionIntersectionType ? "yes" : "no"), "\n";

$c = new C();
var_dump($c->get() instanceof IFoo, $c->get() instanceof IBar);
?>
--EXPECT--
return: IFoo&IBar
is intersection: yes
bool(true)
bool(true)
