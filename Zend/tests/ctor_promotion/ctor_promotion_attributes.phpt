--TEST--
Attributes on promoted properties are assigned to both the property and parameter
--FILE--
<?php

class Test {
    public function __construct(
        #[NonNegative]
        public int $num,
    ) {}
}

$prop = new ReflectionProperty(Test::class, 'num');
var_dump($prop->getAttributes()[0]->getName());

$param = new ReflectionParameter([Test::class, '__construct'], 'num');
var_dump($param->getAttributes()[0]->getName());

?>
--EXPECT--
string(11) "NonNegative"
string(11) "NonNegative"
