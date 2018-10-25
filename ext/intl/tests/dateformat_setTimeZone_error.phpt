--TEST--
IntlDateFormatter::setTimeZone() bad args
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "pt_PT");
ini_set("date.timezone", 'Atlantic/Azores');

$df = new IntlDateFormatter(NULL, 0, 0);

var_dump($df->setTimeZone());
var_dump(datefmt_set_timezone());
var_dump($df->setTimeZone(array()));
var_dump($df->setTimeZone(1, 2));
var_dump($df->setTimeZone('non existing timezone'));
var_dump(datefmt_set_timezone(new stdclass, 'UTC'));

?>
==DONE==
--EXPECTF--
Warning: IntlDateFormatter::setTimeZone() expects exactly 1 parameter, 0 given in %s on line %d

Warning: IntlDateFormatter::setTimeZone(): datefmt_set_timezone: unable to parse input params in %s on line %d
bool(false)

Warning: datefmt_set_timezone() expects exactly 2 parameters, 0 given in %s on line %d

Warning: datefmt_set_timezone(): datefmt_set_timezone: unable to parse input params in %s on line %d
bool(false)

Notice: Array to string conversion in %s on line %d

Warning: IntlDateFormatter::setTimeZone(): datefmt_set_timezone: no such time zone: 'Array' in %s on line %d
bool(false)

Warning: IntlDateFormatter::setTimeZone() expects exactly 1 parameter, 2 given in %s on line %d

Warning: IntlDateFormatter::setTimeZone(): datefmt_set_timezone: unable to parse input params in %s on line %d
bool(false)

Warning: IntlDateFormatter::setTimeZone(): datefmt_set_timezone: no such time zone: 'non existing timezone' in %s on line %d
bool(false)

Warning: datefmt_set_timezone() expects parameter 1 to be IntlDateFormatter, object given in %s on line %d

Warning: datefmt_set_timezone(): datefmt_set_timezone: unable to parse input params in %s on line %d
bool(false)
==DONE==
