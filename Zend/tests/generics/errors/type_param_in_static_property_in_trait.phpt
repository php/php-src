--TEST--
Errors: trait type parameter cannot be used as a static property type
--FILE--
<?php
trait Bag<T> {
    public static null|T $slot = null;
}
?>
--EXPECTF--
Fatal error: Type parameter T is bound to Bag and cannot be used in static context in %s on line %d
