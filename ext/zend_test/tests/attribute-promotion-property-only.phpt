--TEST--
Attribute on promoted property may only target property
--EXTENSIONS--
zend_test
--FILE--
<?php

class AttrTest
{
    public function __construct(
        #[ZendTestPropertyAttribute('foo')] public $param
    ) {}
}

$ref = new ReflectionClass(AttrTest::class);
$attr = $ref->getConstructor()->getParameters()[0]->getAttributes();

var_dump(count($attr));

$attr = $ref->getProperty('param')->getAttributes();

var_dump(count($attr));
var_dump($attr[0]->getName());
var_dump($attr[0]->newInstance()->parameter);

?>
--EXPECTF--
int(0)
int(1)
string(25) "ZendTestPropertyAttribute"
string(3) "foo"
