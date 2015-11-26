--TEST--
Bug #55688 (Crash when calling SessionHandler::gc())
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
html_errors=0
session.save_handler=files
--FILE--
<?php
ini_set('session.save_handler', 'files');
$x = new SessionHandler;
$x->gc(1);
?>
--EXPECTF--
Warning: SessionHandler::gc(): Parent session handler is not open in %s on line %d
