--TEST--
Bug #63185: nextRowset() ignores MySQL errors with native prepared statements
--EXTENSIONS--
pdo
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

$pdo = MySQLPDOTest::factory();
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$pdo->exec('DROP PROCEDURE IF EXISTS test_procedure_error_at_second');
$pdo->exec('CREATE PROCEDURE test_procedure_error_at_second ()
	BEGIN
		SELECT "x" as foo;
		SELECT * FROM no_such_table;
	END');

$pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);
$st = $pdo->query('CALL test_procedure_error_at_second()');
var_dump($st->fetchAll());
try {
    var_dump($st->nextRowset());
} catch (PDOException $e) {
    echo $e->getMessage(), "\n";
}
unset($st);

$pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
$st = $pdo->query('CALL test_procedure_error_at_second()');
var_dump($st->fetchAll());
try {
    var_dump($st->nextRowset());
} catch (PDOException $e) {
    echo $e->getMessage(), "\n";
}
var_dump($st->fetchAll());

?>
--CLEAN--
<?php
require_once __DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc';
$pdo = MySQLPDOTest::factory();
$pdo->query('DROP PROCEDURE IF EXISTS test_procedure_error_at_second');
?>
--EXPECTF--
array(1) {
  [0]=>
  array(2) {
    ["foo"]=>
    string(1) "x"
    [0]=>
    string(1) "x"
  }
}
SQLSTATE[42S02]: Base table or view not found: 1146 Table '%s.no_such_table' doesn't exist
array(1) {
  [0]=>
  array(2) {
    ["foo"]=>
    string(1) "x"
    [0]=>
    string(1) "x"
  }
}
SQLSTATE[42S02]: Base table or view not found: 1146 Table '%s.no_such_table' doesn't exist
array(0) {
}
