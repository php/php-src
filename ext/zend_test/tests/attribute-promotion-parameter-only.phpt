--TEST--
Attribute on promoted property may only target parameter
--EXTENSIONS--
zend_test
--FILE--
<?php

class AttrTest
{
    public function __construct(
        #[ZendTestParameterAttribute('foo')] public $param
    ) {}
}

$ref = new ReflectionClass(AttrTest::class);
$attr = $ref->getConstructor()->getParameters()[0]->getAttributes();

var_dump(count($attr));
var_dump($attr[0]->getName());
var_dump($attr[0]->newInstance()->parameter);

$attr = $ref->getProperty('param')->getAttributes();

var_dump(count($attr));

?>
--EXPECTF--
int(1)
string(26) "ZendTestParameterAttribute"
string(3) "foo"
int(0)
