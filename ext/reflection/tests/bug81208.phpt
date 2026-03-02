--TEST--
Bug #81208: Segmentation fault while create newInstance from attribute
--FILE--
<?php

#[Attribute(Attribute::TARGET_PROPERTY)]
class MyAnnotation
{
    public function __construct(public bool $nullable = false) {}
}

class MyClass {
    #[MyAnnotation(name: "my_name", type: "integer", nullable: asdasdasd)]
    public $property;
}

$z = new ReflectionClass(MyClass::class);
foreach ($z->getProperty("property")->getAttributes() as $attribute) {
    try {
        $attribute->newInstance();
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
Undefined constant "asdasdasd"
