--TEST--
Bug #75434 Wrong reflection for mysqli_fetch_all function
--SKIPIF--
<?php
require_once('skipif.inc');
if (!stristr(mysqli_get_client_info(), 'mysqlnd')) die("skip: only available in mysqlnd");
?>
--FILE--
<?php
$rf = new ReflectionFunction('mysqli_fetch_all');
var_dump($rf->getNumberOfParameters());
var_dump($rf->getNumberOfRequiredParameters());

$rm = new ReflectionMethod('mysqli_result', 'fetch_all');
var_dump($rm->getNumberOfParameters());
var_dump($rm->getNumberOfRequiredParameters());
?>
===DONE===
--EXPECT--
int(2)
int(1)
int(1)
int(0)
===DONE===
