--TEST--
Tests without running the constructor
--EXTENSIONS--
dom
--FILE--
<?php

foreach (['Dom\HTMLDocument', 'Dom\XMLDocument'] as $class) {
    try {
        $rc = new ReflectionClass($class);
        $rc->newInstanceWithoutConstructor();
    } catch (ReflectionException $e) {
        echo $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
Class Dom\HTMLDocument is an internal class marked as final that cannot be instantiated without invoking its constructor
Class Dom\XMLDocument is an internal class marked as final that cannot be instantiated without invoking its constructor
