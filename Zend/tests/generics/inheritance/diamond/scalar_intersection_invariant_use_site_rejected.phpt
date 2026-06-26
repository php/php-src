--TEST--
Diamond + invariant T: scalar bindings at a use-site covariant return are rejected
--FILE--
<?php
interface Box<T> {
    public function get(): T;
}
interface IntBox extends Box<int> {}
interface StrBox extends Box<string> {}

interface FlexBox extends IntBox, StrBox {}
?>
--EXPECTF--
Fatal error: Diamond inheritance of Box::get() in FlexBox would require return type int&string, which is uninhabited; constrain the type parameter with an object bound in %s on line %d
