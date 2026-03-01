--TEST--
Generic class: reject void as generic type argument in extends
--FILE--
<?php
class Box<T> {
    public function __construct(public T $value) {}
}
class VoidBox extends Box<void> {}
?>
--EXPECTF--
Fatal error: void cannot be used as a generic type argument in %s on line %d
