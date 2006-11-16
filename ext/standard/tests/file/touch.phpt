--TEST--
touch() tests
--FILE--
<?php

$filename = dirname(__FILE__)."/touch.dat";

var_dump(touch());
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

echo "Done\n";
?>
--EXPECTF--	
Warning: Wrong parameter count for touch() in %s on line %d
NULL
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

Warning: touch(): Unable to create file /no/such/file/or/directory because No such file or directory in %s on line %d
bool(false)
Done
