--TEST--
Primary constructors: attributes apply to promoted parameters/properties
--FILE--
<?php
#[Attribute(Attribute::TARGET_PARAMETER | Attribute::TARGET_PROPERTY)]
class Col {
    public function __construct(public string $name) {}
}

/** @param int $id */
class Row(
    #[Col('id')] public int $id,
) {}

$class = new ReflectionClass(Row::class);
var_dump($class->getDocComment());
var_dump($class->getConstructor()->getDocComment());

$param = $class->getConstructor()->getParameters()[0];
var_dump($param->getAttributes()[0]->getName());
var_dump($param->getAttributes()[0]->newInstance()->name);

$prop = (new ReflectionProperty(Row::class, 'id'))->getAttributes()[0];
var_dump($prop->newInstance()->name);
?>
--EXPECT--
string(21) "/** @param int $id */"
string(21) "/** @param int $id */"
string(3) "Col"
string(2) "id"
string(2) "id"
