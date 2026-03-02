--TEST--
Test that a notice is emitted when the return type/value of the overriding method is incompatible with the tentative return type/value of the overridden method
--FILE--
<?php
class MyDateTimeZone extends DateTimeZone
{
    public static function listIdentifiers(int $timezoneGroup = DateTimeZone::ALL, ?string $countryCode = null): string
    {
        return "";
    }
}

var_dump(MyDateTimeZone::listIdentifiers());
?>
--EXPECTF--
Deprecated: Return type of MyDateTimeZone::listIdentifiers(int $timezoneGroup = DateTimeZone::ALL, ?string $countryCode = null): string should either be compatible with DateTimeZone::listIdentifiers(int $timezoneGroup = DateTimeZone::ALL, ?string $countryCode = null): array, or the #[\ReturnTypeWillChange] attribute should be used to temporarily suppress the notice in %s on line %d
string(0) ""
