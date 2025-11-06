--TEST--
Reflection shows user-defined BackedEnum::values() when present
--FILE--
<?php

enum E: int {
    case A = 1;

    public static function values(): array {
        return ['custom'];
    }
}

$m = new ReflectionMethod(E::class, 'values');
var_dump($m->isStatic());
var_dump($m->isPublic());
var_dump($m->isInternal());
echo $m->getDeclaringClass()->getName(), "\n";
$proto = $m->getPrototype();
echo $proto->getDeclaringClass()->getName(), "\n";

?>
--EXPECT--
bool(true)
bool(true)
bool(false)
E
BackedEnum

