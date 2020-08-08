--TEST--
Ensure an interface can have protected constants
--FILE--
<?php
interface I {
    protected const FOO = 10;
}

echo "Done\n";
?>
--EXPECT--
Done
