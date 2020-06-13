--TEST--
Ensure a interface can have protected constants
--FILE--
<?php
interface IA {
    protected const FOO = 10;
}

echo "Done\n";
?>
--EXPECT--
Done
