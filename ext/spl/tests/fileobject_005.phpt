--TEST--
SPL: SplFileObject truncate tests
--CREDITS--
Mark Ammann
#Hackday Webtuesday 2008-05-24
--FILE--
<?php

set_include_path(dirname(__DIR__));

$path = __DIR__.DIRECTORY_SEPARATOR.'fileobject_005.txt';
touch($path);

$fo = new SplFileObject('tests'.DIRECTORY_SEPARATOR.'fileobject_005.txt', 'w+', true);
$fo->fwrite("blahlubba");
var_dump($fo->ftruncate(4));

$fo->rewind();
var_dump($fo->fgets(8));

$fo->rewind();
$fo->fwrite("blahlubba");

// This should throw a warning and return NULL since an argument is missing
var_dump($fo->ftruncate());

?>
==DONE==
--CLEAN--
<?php
$path = __DIR__.DIRECTORY_SEPARATOR.'fileobject_005.txt';
unlink($path);
?>
--EXPECTF--
bool(true)

Warning: SplFileObject::fgets() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: SplFileObject::ftruncate() expects exactly 1 parameter, 0 given in %s on line %d
NULL
==DONE==
