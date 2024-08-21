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
    autoload_register_function(array('MyAutoLoader', 'autoLoad'), true);
} catch(\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

// and

$myAutoLoader = new MyAutoLoader();

autoload_register_function(array($myAutoLoader, 'autoLoad'));
autoload_register_function(array($myAutoLoader, 'autoThrow'));

try {
    var_dump(function_exists("foo", true));
} catch(Exception $e) {
    echo 'Exception: ' . $e->getMessage() . "\n";
}

?>
--EXPECT--
autoload_register_function(): Argument #1 ($callback) must be a valid callback, non-static method MyAutoLoader::autoLoad() cannot be called statically
MyAutoLoader::autoLoad(foo)
MyAutoLoader::autoThrow(foo)
Exception: Unavailable
