--TEST--
Turbofish: static-call class type argument must satisfy the class parameter bound
--FILE--
<?php
class Box<T: int> {
    public static function of(T $v): void {}
}
Box::<string>::of("x");
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Type argument 1 to class Box does not satisfy the bound int on parameter T, string given in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
