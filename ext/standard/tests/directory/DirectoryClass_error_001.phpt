--TEST--
Changing Directory::$handle property
--FILE--
<?php

$d = dir(getcwd());
try {
    $d->handle = "Havoc!";
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($d->handle);

$d = dir(getcwd());
try {
    unset($d->handle);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($d->handle);

?>
--EXPECTF--
Cannot modify readonly property Directory::$handle
resource(%d) of type (stream)
Cannot unset readonly property Directory::$handle
resource(%d) of type (stream)
