--TEST--
Diamond + variadic: contravariant T variadic merges as union, exercised at runtime
--FILE--
<?php
interface Sink<in X> { public function sink(X ...$xs): void; }

class A implements Sink<int>, Sink<string> {
    public function sink(int|string ...$xs): void {
        foreach ($xs as $x) echo "A: ", var_export($x, true), "\n";
    }
}

interface IntSink extends Sink<int> {}
interface StrSink extends Sink<string> {}
class B implements IntSink, StrSink {
    public function sink(int|string ...$xs): void {
        foreach ($xs as $x) echo "B: ", var_export($x, true), "\n";
    }
}

interface FlexSink extends IntSink, StrSink {}
class C implements FlexSink {
    public function sink(int|string ...$xs): void {
        foreach ($xs as $x) echo "C: ", var_export($x, true), "\n";
    }
}

(new A)->sink(1, "x", 2);
(new B)->sink("y", 3);
(new C)->sink(4, "z");
?>
--EXPECT--
A: 1
A: 'x'
A: 2
B: 'y'
B: 3
C: 4
C: 'z'
