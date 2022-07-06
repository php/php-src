--TEST--
The default value is null in the parent class method's signature.
--FILE--
<?php
class MyDateTime extends DateTime
{
    public static function createFromFormat()
    {
    }
}
?>
--EXPECTF--
Fatal error: Declaration of MyDateTime::createFromFormat() must be compatible with DateTime::createFromFormat(string $format, string $datetime, ?DateTimeZone $timezone = null) in %s on line %d
