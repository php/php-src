--TEST--
Don't leak when breaking from FilesystemIterator
--FILE--
<?php
$iterator = new FilesystemIterator(__DIR__);
foreach ($iterator as $value) {
    break;
}
?>
===DONE===
--EXPECT--
===DONE===
