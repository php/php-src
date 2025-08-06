--TEST--
IntlTimeZone::toDateTimeZone(): errors
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$tz = IntlTimeZone::createTimeZone('Etc/Unknown');

try {
    var_dump($tz->toDateTimeZone());
} catch (Exception $e) {
    var_dump($e->getMessage());
}

?>
--EXPECTF--
Warning: IntlTimeZone::toDateTimeZone(): DateTimeZone constructor threw exception in %s on line %d
string(66) "DateTimeZone::__construct(): Unknown or bad timezone (Etc/Unknown)"
