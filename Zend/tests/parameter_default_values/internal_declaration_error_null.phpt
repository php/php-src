--TEST--
The default value is null in the parent class method's signature.
--FILE--
<?php
class MyDateTime extends DateTime
{
    public static function createFromFormat(): DateTime|false
    {
    }
}
?>
--EXPECTF--
Fatal error: Declaration of MyDateTime::createFromFormat(): DateTime|false must be compatible with DateTime::createFromFormat(string $format, string $datetime, ?DateTimeZone $timezone = null): DateTime|false in %s on line %d
