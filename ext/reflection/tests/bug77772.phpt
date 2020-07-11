--TEST--
Bug #77772: ReflectionClass::getMethods(null) doesn't work
--FILE--
<?php

class Test {
    public $prop;
    public function method() {}
}

$rc = new ReflectionClass(Test::class);
foreach ($rc->getMethods(null) as $method) {
    var_dump($method->getName());
}
foreach ($rc->getProperties(null) as $prop) {
    var_dump($prop->getName());
}

?>
--EXPECT--
string(6) "method"
string(4) "prop"
