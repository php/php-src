--TEST--
Diamond + trait-use: scalar bindings at a covariant return position are rejected
--FILE--
<?php
trait Producer<out T> {
    public function make(): T { throw new Error('stub'); }
}

class C {
    use Producer<int>, Producer<string>;
}
?>
--EXPECTF--
Fatal error: Diamond inheritance of Producer::make() in C would require return type int&string, which is uninhabited; constrain the type parameter with an object bound in %s on line %d
