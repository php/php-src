--TEST--
ZE2 Validator
--FILE--
<?php

class Test implements Validator
{
    public function isValid($value)
    {
        return $value == 'test';
    }
}

$test = new Test();
var_dump($test->isValid('test')) . PHP_EOL;
var_dump($test->isValid('test1')) . PHP_EOL;
?>
--EXPECT--
bool(true)
bool(false)
