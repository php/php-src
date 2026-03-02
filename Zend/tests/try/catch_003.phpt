--TEST--
Catching an exception in a constructor fired form a static method
--FILE--
<?php

class MyObject
{
    function fail()
    {
        throw new Exception();
    }

    function __construct()
    {
        self::fail();
        echo __METHOD__ . "() Must not be reached\n";
    }

    function __destruct()
    {
        echo __METHOD__ . "() Must not be called\n";
    }
}

try
{
    new MyObject();
}
catch(Exception $e)
{
    echo "Caught\n";
}

?>
--EXPECT--
Caught
