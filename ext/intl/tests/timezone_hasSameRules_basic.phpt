--TEST--
IntlTimeZone::hasSameRules(): basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$lsb = IntlTimeZone::createTimeZone('Europe/Lisbon');
$prt = IntlTimeZone::createTimeZone('Portugal');
$azo = IntlTimeZone::createTimeZone('Atlantic/Azores');

echo "Europe/Lisbon has same rules as itself:\n";
var_dump($lsb->hasSameRules($lsb));

echo "\nEurope/Lisbon has same rules as Portugal:\n";
var_dump($lsb->hasSameRules($prt));

echo "\nEurope/Lisbon has same rules as Atlantic/Azores:\n";
var_dump(intltz_has_same_rules($lsb, $azo));

?>
==DONE==
--EXPECT--
Europe/Lisbon has same rules as itself:
bool(true)

Europe/Lisbon has same rules as Portugal:
bool(true)

Europe/Lisbon has same rules as Atlantic/Azores:
bool(false)
==DONE==