--TEST--
SPL: spl_autoload() with methods
--INI--
include_path=.
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
    spl_autoload_register(array('MyAutoLoader', 'autoLoad'), true);
} catch(\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

// and

$myAutoLoader = new MyAutoLoader();

spl_autoload_register(array($myAutoLoader, 'autoLoad'));
spl_autoload_register(array($myAutoLoader, 'autoThrow'));

try
{
    var_dump(class_exists("TestClass", true));
}
catch(Exception $e)
{
    echo 'Exception: ' . $e->getMessage() . "\n";
}

?>
--EXPECT--
spl_autoload_register(): Argument #1 ($callback) must be a valid callback, non-static method MyAutoLoader::autoLoad() cannot be called statically
MyAutoLoader::autoLoad(TestClass)
MyAutoLoader::autoThrow(TestClass)
Exception: Unavailable
