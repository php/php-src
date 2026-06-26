--TEST--
Diamond + contravariant in T: scalar bindings at a parameter position merge as a union (no rejection)
--FILE--
<?php
interface Sink<in T> {
    public function sink(T $v): void;
}
interface IntSink extends Sink<int> {}
interface StrSink extends Sink<string> {}

interface FlexSink extends IntSink, StrSink {}

$r = new ReflectionClass(FlexSink::class);
echo (string)$r->getMethod('sink')->getParameters()[0]->getType(), "\n";
?>
--EXPECT--
string|int
