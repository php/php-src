--TEST--
GH-11874 (intl causing segfault in docker images)
--EXTENSIONS--
intl
--FILE--
<?php

foreach(IntlCalendar::getKeywordValuesForLocale('calendar', 'fa_IR', true) as $id) {
	var_dump($id);
}

$result = '';
foreach(IntlTimeZone::createTimeZoneIDEnumeration(IntlTimeZone::TYPE_ANY) as $id) {
	$result .= $id;
}

var_dump(strlen($result) > 0);
echo "No crash\n";

?>
--EXPECT--
string(7) "persian"
string(9) "gregorian"
string(7) "islamic"
string(13) "islamic-civil"
string(12) "islamic-tbla"
bool(true)
No crash
