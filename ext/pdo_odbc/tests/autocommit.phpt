--TEST--
PDO ODBC auto commit mode
--EXTENSIONS--
pdo_odbc
--SKIPIF--
<?php
require 'ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--XLEAK--
A bug in msodbcsql causes a memory leak when reconnecting after closing. See GH-12306
--FILE--
<?php
require 'ext/pdo/tests/pdo_test.inc';

$table = 'autocommit_pdo_odbc';

$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
$db->exec("CREATE TABLE {$table} (id INT, name VARCHAR(255))");
unset($db);

$db = new PDO(getenv('PDOTEST_DSN'), getenv('PDOTEST_USER'), getenv('PDOTEST_PASS'), [
    PDO::ATTR_AUTOCOMMIT => 0,
]);

$db->setAttribute(PDO::ATTR_AUTOCOMMIT, 1);
$db->query("INSERT INTO {$table} (id, name) VALUES (1, 'test')");
unset($db);

$db = new PDO(getenv('PDOTEST_DSN'), getenv('PDOTEST_USER'), getenv('PDOTEST_PASS'));

$r = $db->query("SELECT * FROM {$table}");
var_dump($r->fetchAll(PDO::FETCH_ASSOC));

echo "done!";
?>
--CLEAN--
<?php
require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
$db->exec("DROP TABLE IF EXISTS autocommit_pdo_odbc");
?>
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["name"]=>
    string(4) "test"
  }
}
done!
