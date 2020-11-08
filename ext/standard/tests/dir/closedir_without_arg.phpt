--TEST--
Calling closedir() without argument and without opening a directory beforehand
--FILE--
<?php
try {
    closedir();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
No resource supplied
