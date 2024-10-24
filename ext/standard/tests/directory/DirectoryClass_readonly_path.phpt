--TEST--
Changing Directory::$handle property
--FILE--
<?php

$d = dir(__DIR__);
try {
    $d->path = "Havoc!";
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
var_dump($d->path == __DIR__);

try {
    unset($d->path);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
var_dump($d->path == __DIR__);

?>
--EXPECTF--
Error: Cannot modify readonly property Directory::$path
bool(true)
Error: Cannot unset readonly property Directory::$path
bool(true)
