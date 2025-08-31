--TEST--
Test that no notice is emitted when the return type/value of the overriding method is compatible with the tentative return type/value of the overridden method
--FILE--
<?php
class MyDateTimeZone extends DateTimeZone
{
    public static function listIdentifiers(int $timezoneGroup = DateTimeZone::ALL, ?string $countryCode = null): array
    {
        return [];
    }
}

var_dump(MyDateTimeZone::listIdentifiers());
?>
--EXPECT--
array(0) {
}
