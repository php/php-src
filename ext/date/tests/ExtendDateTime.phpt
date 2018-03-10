--TEST--
Extendig DatTime and calling __set_state without args
--CREDITS--
Gabriel Caruso (carusogabriel34@gmail.com)
--FILE--
<?php
class MyDateTime extends DateTime {
    public static function __set_state() {}
}
?>
--EXPECTF--
Warning: Declaration of MyDateTime::__set_state() should be compatible with DateTime::__set_state(array $array) in %s on line %d
