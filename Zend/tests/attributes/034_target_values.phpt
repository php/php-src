--TEST--
Attribute flags are all different, TARGET_ALL includes all targets
--FILE--
<?php

function showFlag(string $name, int $value) {
    $all = Attribute::TARGET_ALL;
    $and = $all & $value;
    echo "Attribute::$name = $value ($all & $value === $and)\n";
}

showFlag("TARGET_CLASS", Attribute::TARGET_CLASS);
showFlag("TARGET_FUNCTION", Attribute::TARGET_FUNCTION);
showFlag("TARGET_METHOD", Attribute::TARGET_METHOD);
showFlag("TARGET_PROPERTY", Attribute::TARGET_PROPERTY);
showFlag("TARGET_CLASS_CONSTANT", Attribute::TARGET_CLASS_CONSTANT);
showFlag("TARGET_PARAMETER", Attribute::TARGET_PARAMETER);
showFlag("TARGET_CONSTANT", Attribute::TARGET_CONSTANT);
showFlag("IS_REPEATABLE", Attribute::IS_REPEATABLE);

$all = Attribute::TARGET_CLASS | Attribute::TARGET_FUNCTION
    | Attribute::TARGET_METHOD | Attribute::TARGET_PROPERTY
    | Attribute::TARGET_CLASS_CONSTANT | Attribute::TARGET_PARAMETER
    | Attribute::TARGET_CONSTANT;
var_dump($all, Attribute::TARGET_ALL, $all === Attribute::TARGET_ALL);

?>
--EXPECT--
Attribute::TARGET_CLASS = 1 (127 & 1 === 1)
Attribute::TARGET_FUNCTION = 2 (127 & 2 === 2)
Attribute::TARGET_METHOD = 4 (127 & 4 === 4)
Attribute::TARGET_PROPERTY = 8 (127 & 8 === 8)
Attribute::TARGET_CLASS_CONSTANT = 16 (127 & 16 === 16)
Attribute::TARGET_PARAMETER = 32 (127 & 32 === 32)
Attribute::TARGET_CONSTANT = 64 (127 & 64 === 64)
Attribute::IS_REPEATABLE = 128 (127 & 128 === 0)
int(127)
int(127)
bool(true)
