--TEST--
SplFileObject::fstat when fstat() has been disabled.
--INI--
disable_functions="fstat"
--FILE--
<?php
$obj = New SplFileObject(__DIR__.'/SplFileObject_testinput.csv');
try {
    var_dump($obj->fstat());
} catch (\RuntimeException $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
fstat() is not defined
