--TEST--
Turbofish: static-call class type argument arity is checked against the class
--FILE--
<?php
class Box<T> {
    public static function of(T $v): void {}
}
Box::<int, string>::of(1);
?>
--EXPECTF--
Fatal error: Uncaught ArgumentCountError: Too many generic type arguments to class Box, 2 passed and exactly 1 expected in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
