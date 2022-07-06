--TEST--
Directory class behaviour.
--FILE--
<?php

echo "\n--> Try all methods with bad handle:\n";
$d = new Directory(getcwd());
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
$d = new Directory(getcwd());
unset($d->handle);

try {
    var_dump($d->read());
} catch (\Error $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump($d->rewind());
} catch (\Error $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump($d->close());
} catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
--> Try all methods with bad handle:
Directory::read(): supplied argument is not a valid Directory resource
Directory::rewind(): supplied argument is not a valid Directory resource
Directory::close(): supplied argument is not a valid Directory resource

--> Try all methods with no handle:
Unable to find my handle property
Unable to find my handle property
Unable to find my handle property
