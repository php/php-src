--TEST--
ReflectionClassConstant::getValue() after taking a reference to the name property
--FILE--
<?php

define('RUNTIME_VALUE', 'abc');

class Test {
    public const int C = RUNTIME_VALUE;
}

$rc = new ReflectionClassConstant(Test::class, 'C');
$name = &$rc->name;
try {
    var_dump($rc->getValue());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot assign string to class constant Test::C of type int
