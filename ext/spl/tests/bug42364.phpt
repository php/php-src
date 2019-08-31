--TEST--
Bug #42364 (Crash when using getRealPath with DirectoryIterator)
--FILE--
<?php
$dir = __DIR__ . '/bug42364';
@mkdir($dir);
touch($dir . '/test');

$count = 0;
$it = new DirectoryIterator($dir);
foreach ($it as $e) {
    $count++;
    $type = gettype($e->getRealPath());
    if ($type != "string" && $type != "unicode") {
        echo $e->getFilename(), " is a ", gettype($e->getRealPath()), "\n";
    }
}

if ($count > 0) {
    echo "Found $count entries!\n";
}
?>
===DONE===
--CLEAN--
<?php
unlink(__DIR__ . '/bug42364/test');
rmdir(__DIR__ . '/bug42364');
?>
--EXPECTF--
Found %i entries!
===DONE===
