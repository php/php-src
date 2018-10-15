--TEST--
MessageFormatter::format() invalid date/time argument
--INI--
date.timezone=Atlantic/Azores
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
if (version_compare(INTL_ICU_VERSION, '4.8') < 0)
	die('skip for ICU 4.8+');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$fmt = <<<EOD
{foo,date}
EOD;

$mf = new MessageFormatter('en_US', $fmt);
var_dump($mf->format(array("foo" => new stdclass())));
--EXPECTF--
Warning: MessageFormatter::format(): msgfmt_format: invalid object type for date/time (only IntlCalendar and DateTimeInterface permitted) in %s on line %d

Warning: MessageFormatter::format(): The argument for key 'foo' cannot be used as a date or time in %s on line %d
bool(false)
