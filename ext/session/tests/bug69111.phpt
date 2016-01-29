--TEST--
Bug #69111 (Crash in SessionHandler::read())
--INI--
session.save_path=
session.save_handler=files
session.name=PHPSESSID
--SKIPIF--
--XFAIL--
It is still a leak
<?php include('skipif.inc'); ?>
--FILE--
<?php
$sh = new SessionHandler;
session_set_save_handler($sh);

$savePath = ini_get('session.save_path');
$sessionName = ini_get('session.name');

// session_start(); // Uncommenting this makes it not crash when reading the session (see below), but it will not return any data.

$sh->open($savePath, $sessionName);
$sh->write("foo", "bar");
$sh->read($id);
$sh->gc(1245);
$sh->close();
?>
--EXPECTF--
Warning: SessionHandler::open(): Session is not active in %s on line 10

Warning: SessionHandler::write(): Session is not active in %s on line 11

Notice: Undefined variable: id in %s on line 12

Warning: SessionHandler::read(): Session is not active in %s on line 12

Warning: SessionHandler::gc(): Session is not active in %s on line 13

Warning: SessionHandler::close(): Session is not active in %s on line 14
