--TEST--
Bug #42364 (Crash when using getRealPath with DirectoryIterator)
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php
$it = new DirectoryIterator(dirname(__FILE__));

foreach ($it as $e) {
    if (gettype($e->getRealPath()) != "string") {
        echo $e->getFilename(), " is a ", gettype($e->getRealPath()), "\n";
    }
}

echo "===DONE==="
?>
--EXPECT--
===DONE===
