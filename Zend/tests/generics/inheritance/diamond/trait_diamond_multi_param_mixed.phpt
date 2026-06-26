--TEST--
Diamond + trait-use: multi-param mixed out T / in U, declared variance drives merge polarity
--FILE--
<?php
interface IFoo {}
interface IBar {}
class FooBar implements IFoo, IBar {}

trait IO<out T : object, in U> {
    public function get(): T { throw new Error('stub'); }
    public function set(U $v): void { echo var_export($v, true), "\n"; }
}

class C {
    use IO<IFoo, int>, IO<IBar, string>;

    public function get(): IFoo&IBar { return new FooBar(); }
    public function set(int|string $v): void { echo "set: ", var_export($v, true), "\n"; }
}

$r = new ReflectionClass(C::class);
echo "get return: ", (string)$r->getMethod('get')->getReturnType(), "\n";
echo "set param:  ", (string)$r->getMethod('set')->getParameters()[0]->getType(), "\n";

$c = new C();
var_dump($c->get() instanceof FooBar);
$c->set(7);
$c->set("hi");
?>
--EXPECT--
get return: IFoo&IBar
set param:  string|int
bool(true)
set: 7
set: 'hi'
