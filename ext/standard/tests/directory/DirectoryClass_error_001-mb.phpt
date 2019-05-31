--TEST--
Directory class behaviour.
--FILE--
<?php

$d = getcwd().PATH_SEPARATOR."私はガラスを食べられます";

mkdir($d);

echo "\n--> Try all methods with bad handle:\n";
$d = new Directory($d);
$d->handle = "Havoc!";
try {
    var_dump($d->read());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($d->rewind());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($d->close());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "\n--> Try all methods with no handle:\n";
$d = new Directory($d);
unset($d->handle);
var_dump($d->read());
var_dump($d->rewind());
var_dump($d->close());

?>
--CLEAN--
<?php
$d = getcwd().PATH_SEPARATOR."私はガラスを食べられます";
rmdir($d);

?>
--EXPECTF--
--> Try all methods with bad handle:
Directory::read(): supplied argument is not a valid Directory resource
Directory::rewind(): supplied argument is not a valid Directory resource
Directory::close(): supplied argument is not a valid Directory resource

--> Try all methods with no handle:

Warning: Directory::read(): Unable to find my handle property in %s on line %d
bool(false)

Warning: Directory::rewind(): Unable to find my handle property in %s on line %d
bool(false)

Warning: Directory::close(): Unable to find my handle property in %s on line %d
bool(false)
