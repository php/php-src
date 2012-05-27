--TEST--
IntlCalendar::getUnknown(): bad arguments
--INI--
date.timezone=Atlantic/Azores
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
if (version_compare(INTL_ICU_VERSION, '49') < 0)
	die('skip for ICU 49+');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$c = new IntlGregorianCalendar(NULL, 'pt_PT');

IntlTimeZone::getUnknown(1);

intltz_get_unknown(1);

--EXPECTF--

Warning: IntlTimeZone::getUnknown() expects exactly 0 parameters, 1 given in %s on line %d

Warning: IntlTimeZone::getUnknown(): intltz_get_unknown: bad arguments in %s on line %d

Warning: intltz_get_unknown() expects exactly 0 parameters, 1 given in %s on line %d

Warning: intltz_get_unknown(): intltz_get_unknown: bad arguments in %s on line %d
