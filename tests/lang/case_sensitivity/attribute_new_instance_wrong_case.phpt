--TEST--
ReflectionAttribute::newInstance() with wrong-case attribute name fails with wrong case
--FILE--
<?php
#[Attribute]
class MyAttr {
    public function __construct(public int $value = 0) {}
}

#[MYATTR(1)]
class Foo {}

$rc = new ReflectionClass(Foo::class);
$attrs = $rc->getAttributes();
foreach ($attrs as $attr) {
    $instance = $attr->newInstance();
    echo get_class($instance) . "\n";
    echo $instance->value . "\n";
}
?>
--EXPECTF--
Fatal error: Uncaught Error: Attribute class "MYATTR" not found in %s:%d
Stack trace:
#0 %s(13): ReflectionAttribute->newInstance()
#1 {main}
  thrown in %s on line %d
