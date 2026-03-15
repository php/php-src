--TEST--
Generic class: reject never as generic type argument in new
--FILE--
<?php
class Box<T> {
    public function __construct(public T $value) {}
}
$b = new Box<never>(42);
?>
--EXPECTF--
Fatal error: never cannot be used as a generic type argument in %s on line %d
