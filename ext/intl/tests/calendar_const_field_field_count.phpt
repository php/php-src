--TEST--
IntlCalendar::FIELD_FIELD_COUNT
--INI--
date.timezone=Atlantic/Azores
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
var_dump(IntlCalendar::FIELD_FIELD_COUNT);
--EXPECTF--
int(%d)
