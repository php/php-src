--TEST--
ReflectionFunction with wrong-case function name throws
--FILE--
<?php
function myFunc(): int {
    return 42;
}

try {
    $rf = new ReflectionFunction("MYFUNC");
} catch (ReflectionException $e) {
    echo $e->getMessage() . "\n";
}

$rf2 = new ReflectionFunction("myFunc");
echo $rf2->getName() . "\n";
?>
--EXPECT--
Function MYFUNC() does not exist
myFunc
