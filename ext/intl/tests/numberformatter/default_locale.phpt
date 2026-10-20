--TEST--
IntlNumberFormatter::createFromSkeleton() with an empty locale uses the default locale
--EXTENSIONS--
intl
--SKIPIF--
<?php
if (!class_exists('IntlNumberFormatter')) {
    die('skip IntlNumberFormatter not available');
}
?>
--INI--
intl.default_locale=en_US
--FILE--
<?php

$skeleton = 'measure-unit/speed-kilometer-per-hour unit-width-full-name';

var_dump(IntlNumberFormatter::createFromSkeleton($skeleton, '')->format(1234.5));

Locale::setDefault('de');

var_dump(IntlNumberFormatter::createFromSkeleton($skeleton, '')->format(1234.5));
var_dump(IntlNumberFormatter::createFromSkeleton($skeleton, 'en_US')->format(1234.5));

?>
--EXPECT--
string(27) "1,234.5 kilometers per hour"
string(28) "1.234,5 Kilometer pro Stunde"
string(27) "1,234.5 kilometers per hour"
