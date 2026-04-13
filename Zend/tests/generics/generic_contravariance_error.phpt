--TEST--
Generic class: contravariance (in) compile error when used in return position
--FILE--
<?php
// in T should not be allowed in method return position
class BadConsumer<in T> {
    public function get(): T { return null; }
}
?>
--EXPECTF--
Fatal error: Contravariant type parameter T of class BadConsumer cannot be used in return type of method BadConsumer::get() (only parameter types allowed) in %s on line %d
