--TEST--
Tests without running the constructor
--EXTENSIONS--
dom
--FILE--
<?php

foreach (['DOM\HTMLDocument', 'DOM\XMLDocument'] as $class) {
    try {
        $rc = new ReflectionClass($class);
        $rc->newInstanceWithoutConstructor();
    } catch (ReflectionException $e) {
        echo $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
Class DOM\HTMLDocument is an internal class marked as final that cannot be instantiated without invoking its constructor
Class DOM\XMLDocument is an internal class marked as final that cannot be instantiated without invoking its constructor
