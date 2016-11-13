--TEST--
PDO_DBLIB: Quoted field names
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require __DIR__ . '/config.inc';
?>
--FILE--
<?php
require __DIR__ . '/config.inc';

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
    int(1)
    ["Another Field"]=>
    string(11) "test_string"
  }
  [1]=>
  array(2) {
    ["My Field"]=>
    int(2)
    ["Another Field"]=>
    string(11) "test_string"
  }
  [2]=>
  array(2) {
    ["My Field"]=>
    int(3)
    ["Another Field"]=>
    string(11) "test_string"
  }
}
Done.
