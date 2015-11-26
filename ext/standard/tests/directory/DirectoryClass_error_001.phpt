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

echo "\n--> Try all methods with wrong number of args:\n";
$d = new Directory(getcwd());
var_dump($d->read(1,2));
var_dump($d->rewind(1,2));
var_dump($d->close(1,2));

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

--> Try all methods with wrong number of args:

Warning: Directory::read() expects at most 1 parameter, 2 given in %s on line %d
NULL

Warning: Directory::rewind() expects at most 1 parameter, 2 given in %s on line %d
NULL

Warning: Directory::close() expects at most 1 parameter, 2 given in %s on line %d
NULL
