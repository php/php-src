--TEST--
PDO MySQL Bug #37445 (Premature stmt object destruction)
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();

$db->setAttribute(PDO :: ATTR_EMULATE_PREPARES, true);
$stmt = $db->prepare("SELECT 1");
$stmt->bindParam(':a', 'b');
?>
--EXPECTF--
Fatal error: Uncaught Error: PDOStatement::bindParam(): Argument #2 ($var) could not be passed by reference in %sbug_37445.php:%d
Stack trace:
#0 {main}
  thrown in %sbug_37445.php on line %d
