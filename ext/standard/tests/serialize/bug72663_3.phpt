--TEST--
Bug #72663 (3): If unserialization fails, don't initialize the session with the result
--SKIPIF--
<?php if (!extension_loaded('session')) die('skip Session extension required'); ?>
--INI--
session.serialize_handler=php_serialize
--FILE--
<?php
session_start();
$sess = 'O:9:"Exception":2:{s:7:"'."\0".'*'."\0".'file";R:1;}';
session_decode($sess);
var_dump($_SESSION);
?>
--EXPECTF--
Notice: session_decode(): Unexpected end of serialized data in %s on line %d
array(0) {
}
