--TEST--
Bug #81407 (shmop_open won't attach and causes php to crash)
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== "Windows") die("skip for Windows only");
if (!extension_loaded("shmop")) die("skip shmop extension not available");
?>
--FILE--
<?php
$a = shmop_open(367504384, 'n', 0664, 262144);
$b = shmop_open(367504385, 'n', 0664, 65536);
if ($b == false) {
	$b = shmop_open(367504385, 'w', 0664, 65536);
}
var_dump($a !== false, $b !== false);
?>
--EXPECT--
bool(true)
bool(true)
