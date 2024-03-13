--TEST--
GH-13680 (Segfault with session_decode and compilation error)
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_cookies=0
session.use_strict_mode=0
session.cache_limiter=
session.serialize_handler=php_serialize
session.save_handler=files
error_reporting=E_ALL
--FILE--
<?php
session_start();
session_decode('foo');
class Test extends DateTime {
    public static function createFromFormat($format, $datetime, $timezone = null): Wrong {}
}
?>
--EXPECTF--
Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d

Fatal error: Could not check compatibility between Test::createFromFormat($format, $datetime, $timezone = null): Wrong and DateTime::createFromFormat(string $format, string $datetime, ?DateTimeZone $timezone = null): DateTime|false, because class Wrong is not available in %s on line %d
