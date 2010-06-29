--TEST--
PDO_DBLIB: Quoted field names
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_dblib')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);

$db->query('CREATE TABLE "Test Table" ("My Field" int, "Another Field" varchar(32) not null default \'test_string\')');
$db->query('INSERT INTO "Test Table" ("My Field") values(1)');
$db->query('INSERT INTO "Test Table" ("My Field") values(2)');
$db->query('INSERT INTO "Test Table" ("My Field") values(3)');
$rs = $db->query('SELECT * FROM "Test Table"');
var_dump($rs->fetchAll(PDO::FETCH_ASSOC));
$db->query('DROP TABLE "Test Table"');
echo "Done.\n";
?>
--EXPECT--
array(3) {
  [0]=>
  array(2) {
    ["my field"]=>
    string(1) "1"
    ["another field"]=>
    string(11) "test_string"
  }
  [1]=>
  array(2) {
    ["my field"]=>
    string(1) "2"
    ["another field"]=>
    string(11) "test_string"
  }
  [2]=>
  array(2) {
    ["my field"]=>
    string(1) "3"
    ["another field"]=>
    string(11) "test_string"
  }
}
Done.
