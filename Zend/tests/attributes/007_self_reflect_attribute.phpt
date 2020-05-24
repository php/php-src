--TEST--
Attributes: attributes on PhpAttribute return itself
--FILE--
<?php

$reflection = new \ReflectionClass(PhpAttribute::class);
$attributes = $reflection->getAttributes();

foreach ($attributes as $attribute) {
    var_dump($attribute->getName());
    var_dump($attribute->getArguments());
    var_dump($attribute->newInstance());
}
--EXPECTF--
string(12) "PhpAttribute"
array(0) {
}
object(PhpAttribute)#3 (0) {
}
