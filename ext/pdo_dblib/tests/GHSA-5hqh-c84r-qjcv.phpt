--TEST--
GHSA-5hqh-c84r-qjcv (Integer overflow in the dblib quoter causing OOB writes)
--EXTENSIONS--
pdo_dblib
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip for 32bit platforms only");
if (PHP_OS_FAMILY === "Windows") die("skip not for Windows because the virtual address space for application is only 2GiB");
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
require __DIR__ . '/config.inc';
getDbConnection();
?>
--INI--
memory_limit=-1
--FILE--
<?php

require __DIR__ . '/config.inc';
$db = getDbConnection();
var_dump($db->quote(str_repeat("'", 2147483646)));

?>
--EXPECT--
bool(false)
