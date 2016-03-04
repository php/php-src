--TEST--
public bool SQLite3::busyTimeout ( int $msecs );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - @phpsp - sao paulo - br
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php
require_once(dirname(__FILE__) . '/new_db.inc');
var_dump($db->busyTimeout(0));
var_dump($db->busyTimeout());
var_dump($db->busyTimeout(""));
var_dump($db->busyTimeout(null));
var_dump($db->busyTimeout(-1000));
var_dump($db->busyTimeout(3.142));
var_dump($db->busyTimeout(3.142e+10));
var_dump($db->busyTimeout(3138343436373434303733373039353531363034));
var_dump($db->busyTimeout(1000));
$db->close();
?>
--EXPECTF--
bool(true)

Warning: SQLite3::busyTimeout() expects parameter %i to be %s, %s given in %s on line %i
NULL

Warning: SQLite3::busyTimeout() expects parameter %i to be %s, %s given in %s on line %i
NULL
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
