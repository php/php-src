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
} catch (Error $ex) {
    var_dump($ex->getCode(), $ex->getMessage());
    echo "\n";
}

try {
    var_dump(intltz_has_same_rules(null, $tz));
} catch (Error $ex) {
    var_dump($ex->getCode(), $ex->getMessage());
    echo "\n";
}
--EXPECT--
int(0)
string(81) "IntlTimeZone::hasSameRules() expects parameter 1 to be IntlTimeZone, string given"

int(0)
string(74) "intltz_has_same_rules() expects parameter 1 to be IntlTimeZone, null given"
