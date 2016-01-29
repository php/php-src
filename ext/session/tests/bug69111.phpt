--TEST--
Bug #69111 Crash in SessionHandler::read()
--SKIPIF--
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
var_dump($sh->read(@$id));
?>
--EXPECTF--
Warning: SessionHandler::read(): The session id is too long or contains illegal characters, valid characters are a-z, A-Z, 0-9 and '-,' in %sbug69111.php on line %d
bool(false)
