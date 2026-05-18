--TEST--
Reflection: cannot instantiate ReflectionGenericTypeParameter directly
--FILE--
<?php
try {
    $rc = new ReflectionClass('ReflectionGenericTypeParameter');
    $rc->newInstanceWithoutConstructor()->__construct();
    echo "no error\n";
} catch (Throwable $e) {
    echo "error: ", get_class($e), "\n";
}
?>
--EXPECT--
error: ReflectionException
