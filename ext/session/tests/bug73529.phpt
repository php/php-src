--TEST--
Bug #73529 session_decode() silently fails on wrong input
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
ob_start();

ini_set("session.serialize_handler", "php_serialize");
session_start();

$result1 = session_decode('foo|s:3:"bar";');
$session1 = $_SESSION;
session_destroy();

ini_set("session.serialize_handler", "php");
session_start();

$result2 = session_decode(serialize(["foo" => "bar"]));
$session2 = $_SESSION;
session_destroy();

echo ob_get_clean();

var_dump($result1);
var_dump($session1);
var_dump($result2);
var_dump($session2);

?>
--EXPECTF--
Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d

Warning: session_destroy(): Trying to destroy uninitialized session in %s on line %d

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d

Warning: session_destroy(): Trying to destroy uninitialized session in %s on line %d
bool(false)
array(0) {
}
bool(false)
array(0) {
}
