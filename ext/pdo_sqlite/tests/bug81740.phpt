--TEST--
Bug #81740 (PDO::quote() may return unquoted string)
--SKIPIF--
<?php
if (!extension_loaded('pdo_sqlite')) print 'skip not loaded';
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--INI--
memory_limit=-1
--FILE--
<?php
$pdo = new PDO("sqlite::memory:");
$string = str_repeat("a", 0x80000000);
var_dump($pdo->quote($string));
?>
--EXPECT--
bool(false)
