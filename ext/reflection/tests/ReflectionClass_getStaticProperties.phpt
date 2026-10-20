--TEST--
ReflectionClass::getStaticProperties()
--FILE--
<?php
class Base {
    public static $pub;
    private static $priv;
    public static mixed $pubTyped;
    private static mixed $privTyped;
}

class Child extends Base {}

$base = new ReflectionClass(Base::class);
$child = new ReflectionClass(Child::class);

echo "Start:\n";
var_dump($base->getStaticProperties());
var_dump($child->getStaticProperties());

echo "\nTyped properties initialized\n:";
Base::$pubTyped = true;
$base->setStaticPropertyValue('privTyped', true);

var_dump($base->getStaticProperties());
var_dump($child->getStaticProperties());
?>
--EXPECT--
Start:
array(2) {
  ["pub"]=>
  NULL
  ["priv"]=>
  NULL
}
array(1) {
  ["pub"]=>
  NULL
}

Typed properties initialized
:array(4) {
  ["pub"]=>
  NULL
  ["priv"]=>
  NULL
  ["pubTyped"]=>
  bool(true)
  ["privTyped"]=>
  bool(true)
}
array(2) {
  ["pub"]=>
  NULL
  ["pubTyped"]=>
  bool(true)
}
