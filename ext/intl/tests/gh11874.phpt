--TEST--
GH-11874 (intl causing segfault with temporaries)
--EXTENSIONS--
intl
--FILE--
<?php

foreach (IntlCalendar::getKeywordValuesForLocale('calendar', 'fa_IR', true) as $id) {
	$id;
}

foreach(IntlTimeZone::createTimeZoneIDEnumeration(IntlTimeZone::TYPE_ANY) as $id) {
	$id;
}

?>
Done
--EXPECT--
Done
