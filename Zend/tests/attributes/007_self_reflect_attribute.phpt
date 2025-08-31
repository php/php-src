--TEST--
Attributes: attributes on Attribute return itself
--FILE--
<?php

$reflection = new \ReflectionClass(Attribute::class);
$attributes = $reflection->getAttributes();

foreach ($attributes as $attribute) {
    var_dump($attribute->getName());
    var_dump($attribute->getArguments());

    $a = $attribute->newInstance();
    var_dump(get_class($a));
    var_dump($a->flags == Attribute::TARGET_CLASS);
}
?>
--EXPECT--
string(9) "Attribute"
array(1) {
  [0]=>
  int(1)
}
string(9) "Attribute"
bool(true)
