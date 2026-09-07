--TEST--
ZE2 catch exception thrown in destructor
--FILE--
<?php

class FailClass
{
    public $fatal;

    function __destruct()
    {
        echo __METHOD__ . "\n";
        throw new exception("FailClass");
        echo "Done: " . __METHOD__ . "\n";
    }
}

try
{
    $a = new FailClass;
    unset($a);
}
catch(Exception $e)
{
    echo $e::class, ': ', $e->getMessage(), "\n";
}

class FatalException extends Exception
{
    function __construct($what)
    {
        echo __METHOD__ . "\n";
        $o = new FailClass;
        unset($o);
        echo "Done: " . __METHOD__ . "\n";
    }
}

try
{
    throw new FatalException("Damn");
}
catch(Exception $e)
{
    echo $e::class, ': ', $e->getMessage(), "\n";
}
catch(FatalException $e)
{
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
FailClass::__destruct
Exception: FailClass
FatalException::__construct
FailClass::__destruct
Exception: FailClass
