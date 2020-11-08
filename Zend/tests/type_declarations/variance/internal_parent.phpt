--TEST--
Internal class as parent
--FILE--
<?php

class Test extends DateTime {
    public static function createFromFormat($format, $datetime, Wrong $timezone = null) { }
}

?>
--EXPECTF--
Fatal error: Could not check compatibility between Test::createFromFormat($format, $datetime, ?Wrong $timezone = null) and DateTime::createFromFormat(string $format, string $datetime, ?DateTimeZone $timezone = null), because class Wrong is not available in %s on line %d
