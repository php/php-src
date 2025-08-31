--TEST--
SPL: SplFileObject truncate tests
--CREDITS--
Mark Ammann
#Hackday Webtuesday 2008-05-24
--FILE--
<?php

$path = __DIR__.DIRECTORY_SEPARATOR.'fileobject_005.txt';
touch($path);

$fo = new SplFileObject($path, 'w+', true);
$fo->fwrite("blahlubba");
var_dump($fo->ftruncate(4));

$fo->rewind();
var_dump($fo->fgets());

$fo->rewind();
$fo->fwrite("blahlubba");

?>
--CLEAN--
<?php
$path = __DIR__.DIRECTORY_SEPARATOR.'fileobject_005.txt';
unlink($path);
?>
--EXPECT--
bool(true)
string(4) "blah"
