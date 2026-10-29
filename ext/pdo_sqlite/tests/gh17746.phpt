--TEST--
GH-17746 (Signed integer overflow when setting ATTR_TIMEOUT)
--EXTENSIONS--
pdo_sqlite
--CREDITS--
YuanchengJiang
--FILE--
<?php
$pdo = new PDO("sqlite::memory:");
var_dump($pdo->setAttribute(PDO::ATTR_TIMEOUT, -1));
var_dump($pdo->setAttribute(PDO::ATTR_TIMEOUT, intdiv(0x7fffffff, 1000) + 1));
?>
--EXPECT--
bool(true)
bool(false)
