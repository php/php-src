--TEST--
PDO_DBLIB: Quoted field names
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
try {
	$db = new PDO( getenv('PDOTEST_DSN'), getenv('PDOTEST_USER'), getenv('PDOTEST_PASS'));
} catch (PDOException $e) {
	die('skip ' . $e->getMessage());
}

?>
--FILE--
<?php
require dirname(__FILE__) . '/config.inc';
$db = new PDO( getenv('PDOTEST_DSN'), getenv('PDOTEST_USER'), getenv('PDOTEST_PASS'));
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
    ["My Field"]=>
    string(1) "1"
    ["Another Field"]=>
    string(11) "test_string"
  }
  [1]=>
  array(2) {
    ["My Field"]=>
    string(1) "2"
    ["Another Field"]=>
    string(11) "test_string"
  }
  [2]=>
  array(2) {
    ["My Field"]=>
    string(1) "3"
    ["Another Field"]=>
    string(11) "test_string"
  }
}
Done.
