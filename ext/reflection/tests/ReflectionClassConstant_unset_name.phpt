--TEST--
Unsetting name on ReflectionClassConstant
--FILE--
<?php

class Test {
    public const C = 1;
}

// This is unsupported and the actual behavior doesn't matter.
// Just make sure it doesn't crash.
$rc = new ReflectionClassConstant(Test::class, 'C');
try {
    unset($rc->name);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

var_dump($rc->getName());

echo $rc, "\n";

?>
--EXPECT--
Cannot unset readonly property ReflectionClassConstant::$name
string(1) "C"
Constant [ public int C ] { 1 }
