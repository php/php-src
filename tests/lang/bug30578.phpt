--TEST--
Bug #30578 (Output buffers flushed before calling __desctruct functions)
--FILE--
<?php

error_reporting(E_ALL);

class Example
{
    function __construct()
    {
        ob_start();
        echo "This should be displayed last.\n";
    }

    function __destruct()
    {
        $buffered_data = ob_get_contents();
        ob_end_clean();

        echo "This should be displayed first.\n";
        echo "Buffered data: $buffered_data";
    }
}

$obj = new Example;

?>
--EXPECT--
This should be displayed first.
Buffered data: This should be displayed last.
