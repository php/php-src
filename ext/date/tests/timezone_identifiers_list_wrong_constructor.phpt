--TEST--
timezone_identifiers_list: ValueError when timezoneGroup is DateTimeZone::PER_COUNTRY
--CREDITS--
Havard Eide <nucleuz@gmail.com>
#PHPTestFest2009 Norway 2009-06-09 \o/
--INI--
date.timezone=UTC
--FILE--
<?php
try {
    var_dump(timezone_identifiers_list(DateTimeZone::PER_COUNTRY));
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump(timezone_identifiers_list(DateTimeZone::PER_COUNTRY, 'A'));
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: timezone_identifiers_list(): Argument #2 ($countryCode) must be a two-letter ISO 3166-1 compatible country code when argument #1 ($timezoneGroup) is DateTimeZone::PER_COUNTRY
ValueError: timezone_identifiers_list(): Argument #2 ($countryCode) must be a two-letter ISO 3166-1 compatible country code when argument #1 ($timezoneGroup) is DateTimeZone::PER_COUNTRY
