--TEST--
SQLite3::createFunction - Test with flags
--EXTENSIONS--
sqlite3
--SKIPIF--
<?php
if (!defined('SQLITE3_DETERMINISTIC')) die('skip SQLITE3_DETERMINISTIC requires SQLite library >= 3.8.3');
?>
--FILE--
<?php

require_once(__DIR__ . '/new_db.inc');

$func = 'strtoupper';
var_dump($db->createfunction($func, $func, 1, SQLITE3_DETERMINISTIC));
var_dump($db->querySingle('SELECT strtoupper("test")'));

$func2 = 'strtolower';
var_dump($db->createfunction($func2, $func2, 1, SQLITE3_DETERMINISTIC));
var_dump($db->querySingle('SELECT strtolower("TEST")'));

var_dump($db->createfunction($func, $func2, 1, SQLITE3_DETERMINISTIC));
var_dump($db->querySingle('SELECT strtoupper("tEst")'));


?>
--EXPECT--
bool(true)
string(4) "TEST"
bool(true)
string(4) "test"
bool(true)
string(4) "test"
