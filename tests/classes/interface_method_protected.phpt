--TEST--
Ensure an interface can have protected methods
--FILE--
<?php
interface I {
    protected function x;
}

echo "Done\n";
?>
--EXPECT--
Done
