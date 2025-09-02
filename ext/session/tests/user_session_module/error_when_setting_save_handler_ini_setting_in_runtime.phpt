--TEST--
Error when setting session.save_handler to user via ini_set
--EXTENSIONS--
session
--FILE--
<?php
ini_set('session.save_handler', 'user');
?>
--EXPECTF--
Warning: ini_set(): Session save handler "user" cannot be set by ini_set() in %s on line %d
