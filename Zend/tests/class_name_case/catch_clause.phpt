--TEST--
Class name with incorrect case fails in catch clause
--FILE--
<?php
class FooException extends Exception {}

// correct case
try {
    throw new FooException('test');
} catch (FooException $e) {
    echo "caught\n";
}

// wrong case - class not found, exception is not caught
try {
    try {
        throw new FooException('test');
    } catch (FOOEXCEPTION $e) {
        echo "should not reach\n";
    }
} catch (FooException $e) {
    echo "not caught by wrong case\n";
}

// wrong case, exception does not match this catch
try {
    try {
        throw new RuntimeException('test');
    } catch (FOOEXCEPTION $e) {
        echo "should not reach\n";
    }
} catch (RuntimeException $e) {
    echo "rethrown\n";
}
?>
--EXPECT--
caught
not caught by wrong case
rethrown
