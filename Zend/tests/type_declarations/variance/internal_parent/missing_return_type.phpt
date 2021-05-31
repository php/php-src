--TEST--
Test that a notice is emitted when the tentative return type of the overridden method is omitted
--FILE--
<?php
class MyDateTimeZone extends DateTimeZone
{
    public static function listIdentifiers(int $timezoneGroup = DateTimeZone::ALL, ?string $countryCode = null)
    {
    }
}
?>
--EXPECTF--
Deprecated: Declaration of MyDateTimeZone::listIdentifiers(int $timezoneGroup = DateTimeZone::ALL, ?string $countryCode = null) should be compatible with DateTimeZone::listIdentifiers(int $timezoneGroup = DateTimeZone::ALL, ?string $countryCode = null): array in %s on line %d
