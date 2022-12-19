--TEST--
Bug #81740 (PDO::quote() may return unquoted string)
--EXTENSIONS--
pdo
pdo_sqlite
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platforms only");
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
