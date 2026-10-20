--TEST--
ZE2 __toString() in __destruct/exception
--FILE--
<?php

class Test
{
    function __toString()
    {
        throw new Exception("Damn!");
        return "Hello\n";
    }

    function __destruct()
    {
        echo $this;
    }
}

try
{
    $o = new Test;
    $o = NULL;
}
catch(Exception $e)
{
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
====DONE====
--EXPECT--
Exception: Damn!
====DONE====
