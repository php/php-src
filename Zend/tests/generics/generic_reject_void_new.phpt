--TEST--
Generic class: reject void as generic type argument in new
--FILE--
<?php
class Box<T> {
    public function __construct(public T $value) {}
}
$b = new Box<void>(42);
?>
--EXPECTF--
Fatal error: void cannot be used as a generic type argument in %s on line %d
