--TEST--
Diamond + variance: all positive paths: contravariant union, covariant intersection
--FILE--
<?php

interface IFoo {}
interface IBar {}
interface IBaz {}

final class FooBar implements IFoo, IBar {}
final class FooBarBaz implements IFoo, IBar, IBaz {}

interface A1Setter<-X> {
    public function set(X $v): void;
}

final class A1Impl implements A1Setter<int>, A1Setter<string> {
    public function set(int|string $v): void { echo "A1: ", var_export($v, true), "\n"; }
}

interface A2Setter<-X> {
    public function set(X $v): void;
}

final class A2Impl implements A2Setter<int>, A2Setter<string>, A2Setter<float> {
    public function set(int|string|float $v): void { echo "A2: ", var_export($v, true), "\n"; }
}

interface A3Setter<-X> {
    public function set(X $v): void;
}

interface A3IntSink extends A3Setter<int> {}
interface A3StrSink extends A3Setter<string> {}
final class A3Impl implements A3IntSink, A3StrSink {
  public function set(int|string $v): void { echo "A3: ", var_export($v, true), "\n"; }
}

interface A5Setter<-X> {
    public function set(X $v): void;
}
interface A5IntL1 extends A5Setter<int>    {}
interface A5IntL2 extends A5IntL1          {}
interface A5StrL1 extends A5Setter<string> {}
interface A5StrL2 extends A5StrL1          {}
final class A5Impl implements A5IntL2, A5StrL2 {
    public function set(int|string $v): void { echo "A5: ", var_export($v, true), "\n"; }
}

class A6Apple {}
class A6Orange {}
interface A6Setter<-X> {
    public function set(X $v): void;
}
final class A6Impl implements A6Setter<A6Apple>, A6Setter<A6Orange> {
    public function set(A6Apple|A6Orange $v): void { echo "A6: ", $v::class, "\n"; }
}

interface A7Sink<-X> {
    public function setOne(X $v): void;
    public function setTwo(X $other): void;
}
final class A7Impl implements A7Sink<int>, A7Sink<string> {
    public function setOne(int|string $v): void { echo "A7.setOne: ", var_export($v, true), "\n"; }
    public function setTwo(int|string $other): void { echo "A7.setTwo: ", var_export($other, true), "\n"; }
}

interface A8Setter<-X> {
    public function set(X $v): void;
}
final class A8Impl implements A8Setter<int>, A8Setter<string> {
    public function set(mixed $v): void { echo "A8: ", var_export($v, true), "\n"; }
}


interface B1Getter<+X : object> {
    public function get(): X;
}
final class B1Impl implements B1Getter<IFoo>, B1Getter<IBar> {
    public function get(): IFoo&IBar { return new FooBar(); }
}

interface B2Getter<+X : object> {
    public function get(): X;
}
final class B2Impl implements B2Getter<IFoo>, B2Getter<IBar>, B2Getter<IBaz> {
    public function get(): IFoo&IBar&IBaz { return new FooBarBaz(); }
}

interface B3Getter<+X : object> {
    public function get(): X;
}
interface B3FooSrc extends B3Getter<IFoo> {}
interface B3BarSrc extends B3Getter<IBar> {}
final class B3Impl implements B3FooSrc, B3BarSrc {
    public function get(): IFoo&IBar { return new FooBar(); }
}

interface B4Getter<+X : object> {
    public function get(): X;
}
abstract class B4ParentSrc implements B4Getter<IFoo> {
    public abstract function get(): IFoo;
}
final class B4Impl extends B4ParentSrc implements B4Getter<IBar> {
    public function get(): IFoo&IBar { return new FooBar(); }
}

interface B5Getter<+X : object> {
    public function get(): X;
}
interface B5FooL1 extends B5Getter<IFoo> {}
interface B5FooL2 extends B5FooL1        {}
interface B5BarL1 extends B5Getter<IBar> {}
interface B5BarL2 extends B5BarL1        {}
final class B5Impl implements B5FooL2, B5BarL2 {
    public function get(): IFoo&IBar { return new FooBar(); }
}

interface B6Getter<+X : object> {
    public function get(): X;
}
final class B6Impl implements B6Getter<IFoo>, B6Getter<IBar> {
    public function get(): FooBar { return new FooBar(); }
}


interface C1IO<+X : object, -Y> {
    public function get(): X;
    public function set(Y $v): void;
}
final class C1Impl implements C1IO<IFoo, int>, C1IO<IBar, string> {
    public function get(): IFoo&IBar { return new FooBar(); }
    public function set(int|string $v): void { echo "C1.set: ", var_export($v, true), "\n"; }
}

interface C2IO<+X : object, -Y> {
    public function get(): X;
    public function set(Y $v): void;
}
interface C2FooInt extends C2IO<IFoo, int>    {}
interface C2BarStr extends C2IO<IBar, string> {}
final class C2Impl implements C2FooInt, C2BarStr {
    public function get(): IFoo&IBar { return new FooBar(); }
    public function set(int|string $v): void { echo "C2.set: ", var_export($v, true), "\n"; }
}

interface C3IO<+X : object, -Y> {
    public function get(): X;
    public function set(Y $v): void;
}
interface C3FooIntL1 extends C3IO<IFoo, int>    {}
interface C3FooIntL2 extends C3FooIntL1          {}
interface C3BarStrL1 extends C3IO<IBar, string> {}
interface C3BarStrL2 extends C3BarStrL1          {}
final class C3Impl implements C3FooIntL2, C3BarStrL2 {
    public function get(): IFoo&IBar { return new FooBar(); }
    public function set(int|string $v): void { echo "C3.set: ", var_export($v, true), "\n"; }
}


interface D1Setter<-X> {
    public function set(X $v): void;
}
interface D1Wrap<-Y> extends D1Setter<Y> {}
final class D1Impl implements D1Wrap<int>, D1Setter<string> {
    public function set(int|string $v): void { echo "D1: ", var_export($v, true), "\n"; }
}


$a1 = new A1Impl();  $a1->set(7);     $a1->set("hi");
$a2 = new A2Impl();  $a2->set(7);     $a2->set("hi");  $a2->set(3.14);
$a3 = new A3Impl();  $a3->set(7);     $a3->set("hi");
$a5 = new A5Impl();  $a5->set(7);     $a5->set("hi");
$a6 = new A6Impl();  $a6->set(new A6Apple());  $a6->set(new A6Orange());
$a7 = new A7Impl();  $a7->setOne(7); $a7->setTwo("hi");
$a8 = new A8Impl();  $a8->set(7);     $a8->set("hi");  $a8->set(3.14);

$b1 = new B1Impl();  var_dump($b1->get() instanceof IFoo, $b1->get() instanceof IBar);
$b2 = new B2Impl();  var_dump($b2->get() instanceof IFoo, $b2->get() instanceof IBar, $b2->get() instanceof IBaz);
$b3 = new B3Impl();  var_dump($b3->get() instanceof IFoo, $b3->get() instanceof IBar);
$b4 = new B4Impl();  var_dump($b4->get() instanceof IFoo, $b4->get() instanceof IBar);
$b5 = new B5Impl();  var_dump($b5->get() instanceof IFoo, $b5->get() instanceof IBar);
$b6 = new B6Impl();  var_dump($b6->get() instanceof FooBar);

$c1 = new C1Impl();  var_dump($c1->get() instanceof FooBar);  $c1->set(7);   $c1->set("hi");
$c2 = new C2Impl();  var_dump($c2->get() instanceof FooBar);  $c2->set(7);   $c2->set("hi");
$c3 = new C3Impl();  var_dump($c3->get() instanceof FooBar);  $c3->set(7);   $c3->set("hi");

$d1 = new D1Impl();  $d1->set(7);     $d1->set("hi");

echo "done\n";
?>
--EXPECT--
A1: 7
A1: 'hi'
A2: 7
A2: 'hi'
A2: 3.14
A3: 7
A3: 'hi'
A5: 7
A5: 'hi'
A6: A6Apple
A6: A6Orange
A7.setOne: 7
A7.setTwo: 'hi'
A8: 7
A8: 'hi'
A8: 3.14
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
C1.set: 7
C1.set: 'hi'
bool(true)
C2.set: 7
C2.set: 'hi'
bool(true)
C3.set: 7
C3.set: 'hi'
D1: 7
D1: 'hi'
done
