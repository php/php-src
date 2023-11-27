--TEST--
Bug #76815: PDOStatement cannot be GCed/closeCursor-ed when a PROCEDURE resultset SIGNAL
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

$pdo = MySQLPDOTest::factory();
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$func = 'bug76815_pdo_mysql_f';
$procedure = 'bug76815_pdo_mysql_p';

$pdo->query("CREATE FUNCTION {$func}() RETURNS VARCHAR(5) DETERMINISTIC BEGIN RETURN 'x12345'; END");
$pdo->query("CREATE PROCEDURE {$procedure}() BEGIN SELECT {$func}(); END");

$st = $pdo->prepare("CALL {$procedure}()");
try {
    $st->execute();
} catch (PDOException $ex) {
    echo $ex->getMessage(), "\n";
}
unset($st);
echo "Ok.\n";
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$pdo = MySQLPDOTest::factory();
$pdo->query('DROP FUNCTION IF EXISTS bug76815_pdo_mysql_f');
$pdo->query('DROP PROCEDURE IF EXISTS bug76815_pdo_mysql_p');
?>
--EXPECT--
SQLSTATE[22001]: String data, right truncated: 1406 Data too long for column 'bug76815_pdo_mysql_f()' at row 1
Ok.
