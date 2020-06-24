--TEST--
Test serialize() & unserialize() functions: objects - ensure that COW references of objects are not serialized separately (unlike other types).
--FILE--
<?php
$x = new stdClass;
$ref = &$x;
var_dump(serialize(array($x, $x)));

$x = 1;
$ref = &$x;
var_dump(serialize(array($x, $x)));

$x = "a";
$ref = &$x;
var_dump(serialize(array($x, $x)));

$x = true;
$ref = &$x;
var_dump(serialize(array($x, $x)));

$x = null;
$ref = &$x;
var_dump(serialize(array($x, $x)));

$x = array();
$ref = &$x;
var_dump(serialize(array($x, $x)));

echo "Done";
?>
--EXPECT--
string(37) "a:2:{i:0;O:8:"stdClass":0:{}i:1;r:2;}"
string(22) "a:2:{i:0;i:1;i:1;i:1;}"
string(30) "a:2:{i:0;s:1:"a";i:1;s:1:"a";}"
string(22) "a:2:{i:0;b:1;i:1;b:1;}"
string(18) "a:2:{i:0;N;i:1;N;}"
string(26) "a:2:{i:0;a:0:{}i:1;a:0:{}}"
Done
