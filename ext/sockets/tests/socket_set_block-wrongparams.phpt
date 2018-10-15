--TEST--
Test if socket_set_block throws E_WARNING with wrong parameters.
--SKIPIF--
<?php
if (!extension_loaded('sockets')) {
    die('SKIP The sockets extension is not loaded.');
}
?>
--FILE--
<?php
socket_set_block(array());
?>
--EXPECTF--
Warning: socket_set_block() expects parameter 1 to be resource, array given in %s on line %d
--CREDITS--
Robin Mehner, robin@coding-robin.de
PHP Testfest Berlin 2009-05-09
