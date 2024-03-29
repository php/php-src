--TEST--
Test unresolvable inheritance check due to unavailable parameter type when the parent has a tentative return type.
--FILE--
<?php

class Test extends DateTime {
    public static function createFromFormat($format, $datetime, ?Wrong $timezone = null): DateTime|false {}
}

?>
--EXPECTF--
Fatal error: Could not check compatibility between Test::createFromFormat($format, $datetime, ?Wrong $timezone = null): DateTime|false and DateTime::createFromFormat(string $format, string $datetime, ?DateTimeZone $timezone = null): DateTime|false, because class Wrong is not available in %s on line %d
