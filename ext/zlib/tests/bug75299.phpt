--TEST--
Bug #75299 Wrong reflection on inflate_init and inflate_add
--SKIPIF--
<?php if (!extension_loaded("zlib")) die("skip"); ?>
--FILE--
<?php
$r = new ReflectionFunction('inflate_init');
var_dump($r->getNumberOfRequiredParameters());
var_dump($r->getNumberOfParameters());
$r = new ReflectionFunction('inflate_add');
var_dump($r->getNumberOfRequiredParameters());
var_dump($r->getNumberOfParameters());
?>
--EXPECT--
int(1)
int(2)
int(2)
int(3)
