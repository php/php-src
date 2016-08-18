--TEST--
Bug #72663: Create an Unexpected Object and Don't Invoke __wakeup() in Deserialization
--FILE--
<?php

ini_set('session.serialize_handler', 'php_serialize');
session_start();
$sess = 'O:9:"Exception":2:{s:7:"'."\0".'*'."\0".'file";R:1;}';
session_decode($sess);
var_dump($_SESSION);
?>
DONE
--EXPECTF--
Notice: session_decode(): Unexpected end of serialized data in %sbug72663_2.php on line %d
array(0) {
}
DONE