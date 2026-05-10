--TEST--
Diamond + nullable: covariant intersection allows null only when every path does
--FILE--
<?php
interface IFoo {}
interface IBar {}
class FooBar implements IFoo, IBar {}

interface Getter<+X : object> { public function get(): ?X; }

interface Flex extends Getter<IFoo>, Getter<IBar> {}
$r = new ReflectionClass(Flex::class);
echo "merged: ", (string)$r->getMethod('get')->getReturnType(), "\n";

class Good1 implements Flex {
    public function get(): (IFoo&IBar)|null { return null; }
}

class Good2 implements Flex {
    public function get(): IFoo&IBar { return new FooBar(); }
}

var_dump((new Good1())->get(), (new Good2())->get() instanceof FooBar);
?>
--EXPECT--
merged: IFoo&IBar|null
NULL
bool(true)
