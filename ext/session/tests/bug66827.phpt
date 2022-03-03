--TEST--
Bug #66827: Session raises E_NOTICE when session name variable is array.
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$_COOKIE[session_name()] = array();
session_start();
echo 'OK';
?>
--EXPECT--
OK
