--TEST--
public bool SQLite3::busyTimeout ( int $msecs );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - @phpsp - sao paulo - br
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php
require_once(dirname(__FILE__) . '/new_db.inc');
var_dump($db->busyTimeout(0));
var_dump($db->busyTimeout(null));
var_dump($db->busyTimeout(-1000));
var_dump($db->busyTimeout(1000));
$db->close();
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
