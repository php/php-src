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
try {
	var_dump($tz->hasSameRules('foo'));
} catch (EngineException $ex) {
	var_dump($ex->getCode(), $ex->getMessage());
	echo "\n";
}

try {
	var_dump(intltz_has_same_rules(null, $tz));
} catch (EngineException $ex) {
	var_dump($ex->getCode(), $ex->getMessage());
	echo "\n";
}

--EXPECT--
int(1)
string(99) "Argument 1 passed to IntlTimeZone::hasSameRules() must be an instance of IntlTimeZone, string given"

int(1)
string(92) "Argument 1 passed to intltz_has_same_rules() must be an instance of IntlTimeZone, null given"

