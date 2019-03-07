--TEST--
Directory class behaviour.
--FILE--
<?php

echo "\n--> Try all methods with bad handle:\n";
$d = new Directory(getcwd());
$d->handle = "Havoc!";
var_dump($d->read());
var_dump($d->rewind());
var_dump($d->close());

echo "\n--> Try all methods with no handle:\n";
$d = new Directory(getcwd());
unset($d->handle);
var_dump($d->read());
var_dump($d->rewind());
var_dump($d->close());

?>
--EXPECTF--
--> Try all methods with bad handle:

Warning: Directory::read(): supplied argument is not a valid Directory resource in %s on line %d
bool(false)

Warning: Directory::rewind(): supplied argument is not a valid Directory resource in %s on line %d
bool(false)

Warning: Directory::close(): supplied argument is not a valid Directory resource in %s on line %d
bool(false)

--> Try all methods with no handle:

Warning: Directory::read(): Unable to find my handle property in %s on line %d
bool(false)

Warning: Directory::rewind(): Unable to find my handle property in %s on line %d
bool(false)

Warning: Directory::close(): Unable to find my handle property in %s on line %d
bool(false)
