--TEST--
Class name with incorrect case in catch clause is not caught
--FILE--
<?php
class FooException extends Exception {}

try {
    try {
        throw new FooException('test');
    } catch (FOOEXCEPTION $e) {
        echo "should not reach\n";
    }
} catch (FooException $e) {
    echo "not caught by wrong case\n";
}
?>
--EXPECT--
not caught by wrong case
