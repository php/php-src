--TEST--
SPL function autoload with exceptions
--FILE--
<?php
class MyAutoLoader {

        function autoLoad($name)
        {
            echo __METHOD__ . "($name)\n";
        }

        function autoThrow($name)
        {
            echo __METHOD__ . "($name)\n";
            throw new Exception("Unavailable");
        }
}

$myAutoLoader = new MyAutoLoader();

spl_autoload_register($myAutoLoader->autoLoad(...), type: SPL_AUTOLOAD_FUNCTION);
spl_autoload_register($myAutoLoader->autoThrow(...), type: SPL_AUTOLOAD_FUNCTION);

try
{
    var_dump(function_exists("TestFunc", true));
}
catch(Exception $e)
{
    echo 'Exception: ' . $e->getMessage() . "\n";
}
?>
--EXPECT--
MyAutoLoader::autoLoad(TestFunc)
MyAutoLoader::autoThrow(TestFunc)
Exception: Unavailable
