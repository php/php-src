--TEST--
static method named 'from' remains usable
--FILE--
<?php
class C {
    public static function from($x) { echo $x; }
}
C::from(123);
?>
--EXPECT--
123
