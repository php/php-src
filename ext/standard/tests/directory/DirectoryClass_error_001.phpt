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

Warning: Directory::read(): supplied argument is not a valid Directory resource in %s on line 6
bool(false)

Warning: Directory::rewind(): supplied argument is not a valid Directory resource in %s on line 7
bool(false)

Warning: Directory::close(): supplied argument is not a valid Directory resource in %s on line 8
bool(false)

--> Try all methods with no handle:

Warning: Directory::read(): Unable to find my handle property in %s on line 13
bool(false)

Warning: Directory::rewind(): Unable to find my handle property in %s on line 14
bool(false)

Warning: Directory::close(): Unable to find my handle property in %s on line 15
bool(false)

--> Try all methods with wrong number of args:

Warning: Wrong parameter count for Directory::read() in %s on line 19
NULL

Warning: Wrong parameter count for Directory::rewind() in %s on line 20
NULL

Warning: Wrong parameter count for Directory::close() in %s on line 21
NULL