--TEST--
The default value is a class constant in the parent class method's signature.
--FILE--
<?php
class MyDateTimeZone extends DateTimeZone
{
    public static function listIdentifiers(): array
    {
    }
}
?>
--EXPECTF--
Fatal error: Declaration of MyDateTimeZone::listIdentifiers(): array must be compatible with DateTimeZone::listIdentifiers(int $timezoneGroup = DateTimeZone::ALL, ?string $countryCode = null): array in %s on line %d
