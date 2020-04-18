--TEST--
Bug #74541 Wrong reflection on session_start()
--SKIPIF--
<?php
include('skipif.inc');
?>
--FILE--
<?php
$r = new ReflectionFunction('session_start');
var_dump($r->getNumberOfParameters());
var_dump($r->getNumberOfRequiredParameters());
?>
--EXPECT--
int(1)
int(0)
