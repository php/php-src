--TEST--
ReflectionFunction::isDisabled
--CREDITS--
Stefan Koopmanschap <stefan@phpgg.nl>
TestFest PHP|Tek
--INI--
disable_functions=is_file
--FILE--
<?php
try {
    $rf = new ReflectionFunction('is_file');
    var_dump($rf->isDisabled());
} catch (ReflectionException $e) {
    echo $e->getMessage(), "\n";
}

$rf = new ReflectionFunction('is_string');
var_dump($rf->isDisabled());
?>
--EXPECTF--
Function is_file() does not exist

Deprecated: Method ReflectionFunction::isDisabled() is deprecated in %s on line %d
bool(false)
