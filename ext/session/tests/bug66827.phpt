--TEST--
Bug #66827: Session raises E_NOTICE when session name variable is array.
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php
$_COOKIE[session_name()] = array();
session_start();
echo 'OK';
?>
--EXPECT--
OK
