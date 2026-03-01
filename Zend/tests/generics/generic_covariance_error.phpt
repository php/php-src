--TEST--
Generic class: covariance (out) compile error when used in parameter position
--FILE--
<?php
// out T should not be allowed in method parameter position
class BadBox<out T> {
    public function set(T $value): void {}
}
?>
--EXPECTF--
Fatal error: Covariant type parameter T of class BadBox cannot be used in parameter $value of method BadBox::set() (only return types allowed) in %s on line %d
