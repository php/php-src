--TEST--
Internal class as parent
--FILE--
<?php

class Test extends DateTime {
    public static function createFromFormat($format, $time, Wrong $timezone = null) { }
}

?>
--EXPECTF--
Warning: Could not check compatibility between Test::createFromFormat($format, $time, ?Wrong $timezone = NULL) and DateTime::createFromFormat($format, $time, ?DateTimeZone $object = NULL), because class Wrong is not available in %s on line %d
