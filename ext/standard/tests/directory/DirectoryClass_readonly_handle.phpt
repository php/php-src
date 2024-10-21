--TEST--
Changing Directory::$handle property
--FILE--
<?php

$d = dir(__DIR__);
try {
    $d->handle = "Havoc!";
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
var_dump($d->handle);

try {
    unset($d->handle);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
var_dump($d->handle);

?>
--EXPECTF--
Error: Cannot modify readonly property Directory::$handle
resource(%d) of type (stream)
Error: Cannot unset readonly property Directory::$handle
resource(%d) of type (stream)
