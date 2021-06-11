--TEST--
IntlTimeZone::hasSameRules(): errors
--EXTENSIONS--
intl
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
?>
--EXPECT--
int(0)
string(93) "IntlTimeZone::hasSameRules(): Argument #1 ($other) must be of type IntlTimeZone, string given"

int(0)
string(89) "intltz_has_same_rules(): Argument #1 ($timezone) must be of type IntlTimeZone, null given"
