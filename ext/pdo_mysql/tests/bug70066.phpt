--TEST--
Bug #70066: Unexpected "Cannot execute queries while other unbuffered queries"
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_mysql')) die('skip not loaded');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php

require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

$pdo = MySQLPDOTest::factory();
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, 0);

$db = $pdo->query('SELECT DATABASE()')->fetchColumn(0);
// USE is not supported in the prepared statement protocol,
// so this will fall back to emulation.
$pdo->query('USE ' . $db);

?>
===DONE===
--EXPECT--
===DONE===
