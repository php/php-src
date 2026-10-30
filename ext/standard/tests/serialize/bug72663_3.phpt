--TEST--
Bug #72663 (3): If unserialization fails, don't initialize the session with the result
--EXTENSIONS--
session
--INI--
session.serialize_handler=php_serialize
--FILE--
<?php
session_start();
$sess = 'O:9:"Exception":2:{s:7:"'."\0".'*'."\0".'file";s:0:"";}';
session_decode($sess);
var_dump($_SESSION);
?>
--EXPECTF--
Warning: session_decode(): Unexpected end of serialized data in %s on line %d

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
array(0) {
}
