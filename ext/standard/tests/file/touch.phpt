--TEST--
touch() tests
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. only for Non Windows.');
}
?>
--FILE--
<?php

// This doesn't work for windows, time, atime usage results in very different
// output to linux. This could be a php.net bug on windows or a windows querk.
$filename = __DIR__."/touch.dat";

var_dump(touch($filename));
var_dump(filemtime($filename));
@unlink($filename);
var_dump(touch($filename, 101));
var_dump(filemtime($filename));

@unlink($filename);
var_dump(touch($filename, -1));
var_dump(filemtime($filename));

@unlink($filename);
var_dump(touch($filename, 100, 100));
var_dump(filemtime($filename));

@unlink($filename);
var_dump(touch($filename, 100, -100));
var_dump(filemtime($filename));

var_dump(touch("/no/such/file/or/directory"));

@unlink($filename);

try {
    touch("/no/such/file/or/directory", null, 1599492068);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

echo "Done\n";
?>
--EXPECTF--
bool(true)
int(%d)
bool(true)
int(101)
bool(true)
int(%i)
bool(true)
int(100)
bool(true)
int(100)

Warning: touch(): Unable to create file /no/such/file/or/directory because %s in %s on line %d
bool(false)
touch(): Argument #2 ($mtime) cannot be null when argument #3 ($atime) is an integer
Done
