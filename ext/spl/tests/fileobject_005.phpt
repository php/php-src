--TEST--
SPL: SplFileObject truncate tests
--CREDITS--
Mark Ammann
#Hackday Webtuesday 2008-05-24
--FILE--
<?php

set_include_path(dirname(dirname(__FILE__)));

$path = dirname(__FILE__).DIRECTORY_SEPARATOR.'fileobject_005.txt';
touch($path);

$fo = new SplFileObject('tests'.DIRECTORY_SEPARATOR.'fileobject_005.txt', 'w+', true);
$fo->fwrite("blahlubba");
var_dump($fo->ftruncate(4));

$fo->rewind();
var_dump($fo->fgets());

$fo->rewind();
$fo->fwrite("blahlubba");

?>
==DONE==
--CLEAN--
<?php
$path = dirname(__FILE__).DIRECTORY_SEPARATOR.'fileobject_005.txt';
unlink($path);
?>
--EXPECTF--
bool(true)

Warning: SplFileObject::fgets() expects exactly 0 parameters, 1 given in %s on line %d
NULL
==DONE==
