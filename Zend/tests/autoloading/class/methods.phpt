--TEST--
Autoloader is a method
--FILE--
<?php

class MyAutoLoader {
    function autoLoad($className)
    {
        echo __METHOD__ . "($className)\n";
    }

    function autoThrow($className)
    {
        echo __METHOD__ . "($className)\n";
        throw new Exception("Unavailable");
    }
}

try {
    autoload_register_class(array('MyAutoLoader', 'autoLoad'), true);
} catch(\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

// and

$myAutoLoader = new MyAutoLoader();

autoload_register_class(array($myAutoLoader, 'autoLoad'));
autoload_register_class(array($myAutoLoader, 'autoThrow'));

try {
    var_dump(class_exists("TestClass", true));
} catch(Exception $e) {
    echo 'Exception: ' . $e->getMessage() . "\n";
}

?>
--EXPECT--
autoload_register_class(): Argument #1 ($callback) must be a valid callback, non-static method MyAutoLoader::autoLoad() cannot be called statically
MyAutoLoader::autoLoad(TestClass)
MyAutoLoader::autoThrow(TestClass)
Exception: Unavailable
