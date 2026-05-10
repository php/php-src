--TEST--
Diamond + trait-use: invariant T (no marker) with T in both positions merges use-site
--FILE--
<?php
trait Pipe<T : object> {
    public function pipe(T $x): T { return $x; }
}

interface IFoo {}
interface IBar {}
class FooBar implements IFoo, IBar {}

class C {
    use Pipe<IFoo>, Pipe<IBar>;
}

$r = new ReflectionClass(C::class);
$m = $r->getMethod('pipe');
echo "param:  ", (string)$m->getParameters()[0]->getType(), "\n";
echo "return: ", (string)$m->getReturnType(), "\n";
echo "param is union:        ", ($m->getParameters()[0]->getType() instanceof ReflectionUnionType ? "yes" : "no"), "\n";
echo "return is intersection: ", ($m->getReturnType() instanceof ReflectionIntersectionType ? "yes" : "no"), "\n";

$c = new C();
var_dump($c->pipe(new FooBar()) instanceof FooBar);
?>
--EXPECT--
param:  IFoo|IBar
return: IFoo&IBar
param is union:        yes
return is intersection: yes
bool(true)
