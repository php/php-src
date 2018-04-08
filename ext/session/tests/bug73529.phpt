--TEST--
Bug #73529 session_decode() silently fails on wrong input
--XFAIL--
session_decode() does not return proper status.
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ini_set("session.serialize_handler", "php_serialize");
session_start();

$result1 = session_decode(serialize(["foo" => "bar"]));
$session1 = $_SESSION;
session_destroy();

ini_set("session.serialize_handler", "php");
session_start();

$result2 = session_decode(serialize(["foo" => "bar"]));
$session2 = $_SESSION;
session_destroy();

var_dump($result1);
var_dump($session1);
var_dump($result2);
var_dump($session2);

?>
--EXPECT--
bool(true)
array(1) {
  ["foo"]=>
  string(3) "bar"
}
bool(false)
array(0) {
}
