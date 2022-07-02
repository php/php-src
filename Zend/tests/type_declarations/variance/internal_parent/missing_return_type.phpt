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
Deprecated: Return type of MyDateTimeZone::listIdentifiers(int $timezoneGroup = DateTimeZone::ALL, ?string $countryCode = null) should either be compatible with DateTimeZone::listIdentifiers(int $timezoneGroup = DateTimeZone::ALL, ?string $countryCode = null): array, or the #[\ReturnTypeWillChange] attribute should be used to temporarily suppress the notice in %s on line %d
