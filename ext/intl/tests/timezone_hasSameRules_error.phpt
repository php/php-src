--TEST--
IntlTimeZone::hasSameRules(): errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

function error_handler($errno, $errstr, $errfile, $errline)
{
	var_dump($errno, $errstr);
	return true;
}
set_error_handler("error_handler");

$tz = IntlTimeZone::createTimeZone('Europe/Lisbon');
var_dump($tz->hasSameRules('foo'));

var_dump(intltz_has_same_rules(null, $tz));

--EXPECT--
int(4096)
string(99) "Argument 1 passed to IntlTimeZone::hasSameRules() must be an instance of IntlTimeZone, string given"
int(2)
string(81) "IntlTimeZone::hasSameRules() expects parameter 1 to be IntlTimeZone, string given"
int(2)
string(66) "IntlTimeZone::hasSameRules(): intltz_has_same_rules: bad arguments"
bool(false)
int(4096)
string(92) "Argument 1 passed to intltz_has_same_rules() must be an instance of IntlTimeZone, null given"
int(2)
string(74) "intltz_has_same_rules() expects parameter 1 to be IntlTimeZone, null given"
int(2)
string(61) "intltz_has_same_rules(): intltz_has_same_rules: bad arguments"
bool(false)
