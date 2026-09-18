--TEST--
Value class readonly semantics do not recursively freeze objects or array references
--FILE--
<?php
value class Container {
    public function __construct(public object $object, public array $array) {}
}
$number = 1;
$object = new stdClass;
$object->number = 1;
$container = new Container($object, [&$number]);
$object->number = 2;
$number = 2;
var_dump($container->object->number, $container->array[0]);
?>
--EXPECT--
int(2)
int(2)
