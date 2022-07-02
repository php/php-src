--TEST--
Ensure a interface can have public constants
--FILE--
<?php
interface IA {
    public const FOO = 10;
}

echo "Done\n";
?>
--EXPECT--
Done
