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
Fatal error: Method MyDateTime::__set_state() must take exactly 1 argument in %s on line %d
