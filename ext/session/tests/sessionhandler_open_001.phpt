--TEST--
Testing repated SessionHandler::open() calls
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ini_set('session.save_handler', 'files');
$x = new SessionHandler;
$x->open('','');
$x->open('','');
$x->open('','');
$x->open('','');

print "Done!\n";

?>
--EXPECTF--
Warning: SessionHandler::open(): Session is not active in %s on line 5

Warning: SessionHandler::open(): Session is not active in %s on line 6

Warning: SessionHandler::open(): Session is not active in %s on line 7

Warning: SessionHandler::open(): Session is not active in %s on line 8
Done!
