--TEST--
Diamond + variance: an interface can itself diamond-inherit a generic ancestor
--FILE--
<?php
interface IFoo {}
interface IBar {}
interface IBaz {}

final class FooBar implements IFoo, IBar {}
final class FooBarBaz implements IFoo, IBar, IBaz {}

interface I1Setter<-X> { public function set(X $v): void; }
interface I1IntSink extends I1Setter<int> {}
interface I1StrSink extends I1Setter<string> {}
interface I1Flex extends I1IntSink, I1StrSink {}

final class I1Impl implements I1Flex {
    public function set(int|string $v): void { echo "I1: ", var_export($v, true), "\n"; }
}

interface I2Setter<-X> { public function set(X $v): void; }
interface I2Flex extends I2Setter<int>, I2Setter<string> {}

final class I2Impl implements I2Flex {
    public function set(int|string $v): void { echo "I2: ", var_export($v, true), "\n"; }
}

interface I3Setter<-X> { public function set(X $v): void; }
interface I3Flex extends I3Setter<int>, I3Setter<string>, I3Setter<float> {}

final class I3Impl implements I3Flex {
    public function set(int|string|float $v): void { echo "I3: ", var_export($v, true), "\n"; }
}

interface I4Getter<+X : object> { public function get(): X; }
interface I4FooSrc extends I4Getter<IFoo> {}
interface I4BarSrc extends I4Getter<IBar> {}
interface I4Flex extends I4FooSrc, I4BarSrc {}

final class I4Impl implements I4Flex {
    public function get(): IFoo&IBar { return new FooBar(); }
}

interface I5Getter<+X : object> { public function get(): X; }
interface I5Flex extends I5Getter<IFoo>, I5Getter<IBar>, I5Getter<IBaz> {}

final class I5Impl implements I5Flex {
    public function get(): IFoo&IBar&IBaz { return new FooBarBaz(); }
}

interface I6IO<+X : object, -Y> {
    public function get(): X;
    public function set(Y $v): void;
}
interface I6Flex extends I6IO<IFoo, int>, I6IO<IBar, string> {}

final class I6Impl implements I6Flex {
    public function get(): IFoo&IBar { return new FooBar(); }
    public function set(int|string $v): void { echo "I6.set: ", var_export($v, true), "\n"; }
}

interface I7Setter<-X> { public function set(X $v): void; }
interface I7L1A extends I7Setter<int>    {}
interface I7L2A extends I7L1A             {}
interface I7L1B extends I7Setter<string> {}
interface I7L2B extends I7L1B             {}
interface I7Flex extends I7L2A, I7L2B {}

final class I7Impl implements I7Flex {
    public function set(int|string $v): void { echo "I7: ", var_export($v, true), "\n"; }
}

interface I8Setter<-X> { public function set(X $v): void; }
interface I8Flex extends I8Setter<int>, I8Setter<string> {}

final class I8Impl implements I8Flex {
    public function set(mixed $v): void { echo "I8: ", var_export($v, true), "\n"; }
}

interface I9Getter<+X : object> { public function get(): X; }
interface I9Flex extends I9Getter<IFoo>, I9Getter<IBar> {}

final class I9Impl implements I9Flex {
    public function get(): FooBar { return new FooBar(); }
}

(new I1Impl)->set(7);   (new I1Impl)->set("hi");
(new I2Impl)->set(7);   (new I2Impl)->set("hi");
(new I3Impl)->set(7);   (new I3Impl)->set("hi");   (new I3Impl)->set(3.14);
var_dump((new I4Impl)->get() instanceof IFoo, (new I4Impl)->get() instanceof IBar);
var_dump((new I5Impl)->get() instanceof IFoo, (new I5Impl)->get() instanceof IBar, (new I5Impl)->get() instanceof IBaz);
$i6 = new I6Impl; var_dump($i6->get() instanceof FooBar); $i6->set(1); $i6->set("x");
(new I7Impl)->set(7);   (new I7Impl)->set("hi");
(new I8Impl)->set(7);   (new I8Impl)->set("hi");   (new I8Impl)->set(3.14);
var_dump((new I9Impl)->get() instanceof FooBar);

echo "done\n";
?>
--EXPECT--
I1: 7
I1: 'hi'
I2: 7
I2: 'hi'
I3: 7
I3: 'hi'
I3: 3.14
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
I6.set: 1
I6.set: 'x'
I7: 7
I7: 'hi'
I8: 7
I8: 'hi'
I8: 3.14
bool(true)
done
