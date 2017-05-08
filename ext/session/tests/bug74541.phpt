--TEST--
Bug #74541 Wrong reflection on session_start()
--SKIPIF--
<?php
include('skipif.inc');
if (!extension_loaded('reflection')) die("skip");
?>
--FILE--
<?php
$r = new ReflectionFunction('session_start');
var_dump($r->getNumberOfParameters());
var_dump($r->getNumberOfRequiredParameters());
?>
===DONE===
--EXPECT--
int(1)
int(0)
===DONE===
