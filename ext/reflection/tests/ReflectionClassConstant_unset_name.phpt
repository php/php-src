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
unset($rc->name);
try {
    var_dump($rc->getName());
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    echo $rc, "\n";
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Typed property ReflectionClassConstant::$name must not be accessed before initialization
Typed property ReflectionClassConstant::$name must not be accessed before initialization
