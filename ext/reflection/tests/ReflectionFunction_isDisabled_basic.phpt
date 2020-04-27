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
    $rc = new ReflectionFunction('is_file');
    var_dump($rc->isDisabled());
} catch (ReflectionException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Function is_file() does not exist
