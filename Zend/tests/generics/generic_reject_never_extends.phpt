--TEST--
Generic class: reject never as generic type argument in extends
--FILE--
<?php
class Box<T> {
    public function __construct(public T $value) {}
}
class NeverBox extends Box<never> {}
?>
--EXPECTF--
Fatal error: never cannot be used as a generic type argument in %s on line %d
