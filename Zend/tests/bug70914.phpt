--TEST--
Bug #70895 null ptr deref and segfault with crafted callable
--SKIPIF--
<?php
if (!extension_loaded("pdo_sqlite")) {
	die("skip pdo_sqlite required");
}
?>
--FILE--
<?php

$db = new PDO('sqlite::memory:');
$st = $db->query('SELECT 1');
$re = $st->fetchObject('%Z');
?>
--EXPECTREGEX--
Fatal error: Class '%Z' not found in .+bug70914.php on line \d+
