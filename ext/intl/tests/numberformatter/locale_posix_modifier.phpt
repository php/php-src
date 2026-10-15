--TEST--
IntlNumberFormatter preserves the region of a locale with a POSIX modifier
--EXTENSIONS--
intl
--SKIPIF--
<?php
if (!class_exists('IntlNumberFormatter')) {
    die('skip IntlNumberFormatter not available');
}
?>
--FILE--
<?php
$skeleton = 'measure-unit/length-kilometer unit-width-full-name';

$plain = IntlNumberFormatter::createFromSkeleton($skeleton, 'pt_PT');
$modified = IntlNumberFormatter::createFromSkeleton($skeleton, 'pt_PT@euro');

var_dump($plain->format(12345.5) === $modified->format(12345.5));
?>
--EXPECT--
bool(true)
