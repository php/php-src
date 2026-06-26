--TEST--
Diamond + covariant out T: scalar bindings in a covariant return position are rejected
--FILE--
<?php
interface Producer<out T> {
    public function make(): T;
}
interface IntProducer extends Producer<int> {}
interface StrProducer extends Producer<string> {}

interface FlexProducer extends IntProducer, StrProducer {}
?>
--EXPECTF--
Fatal error: Diamond inheritance of Producer::make() in FlexProducer would require return type int&string, which is uninhabited; constrain the type parameter with an object bound in %s on line %d
