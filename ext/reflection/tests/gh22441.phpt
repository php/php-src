--TEST--
GH-22441 (ReflectionClass::hasProperty()/getProperty() ignore dynamic properties shadowing a private parent property)
--FILE--
<?php

class Base {
    private mixed $shadow;
    private mixed $onlyBase;
}

#[AllowDynamicProperties]
class Child extends Base {}

$o = new Child();
$o->shadow = true;
$o->noShadow = true;

$r = new ReflectionObject($o);

echo "hasProperty:\n";
echo "shadow (dynamic over private parent): ";   var_dump($r->hasProperty('shadow'));
echo "noShadow (plain dynamic): ";               var_dump($r->hasProperty('noShadow'));
echo "onlyBase (private parent, no dynamic): ";  var_dump($r->hasProperty('onlyBase'));

echo "\ngetProperty:\n";
foreach (['shadow', 'noShadow', 'onlyBase'] as $name) {
    try {
        $p = $r->getProperty($name);
        printf("%s: %s::\$%s\n", $name, $p->getDeclaringClass()->getName(), $p->getName());
    } catch (ReflectionException $e) {
        printf("%s: %s\n", $name, $e->getMessage());
    }
}
?>
--EXPECT--
hasProperty:
shadow (dynamic over private parent): bool(true)
noShadow (plain dynamic): bool(true)
onlyBase (private parent, no dynamic): bool(false)

getProperty:
shadow: Child::$shadow
noShadow: Child::$noShadow
onlyBase: Property Child::$onlyBase does not exist
