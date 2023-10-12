--TEST--
PDO_DBLIB: Quoted field names
--EXTENSIONS--
pdo_dblib
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
?>
--FILE--
<?php
require __DIR__ . '/config.inc';

$db->query('CREATE TABLE "Test Table47588" ("My Field" int, "Another Field" varchar(32) not null default \'test_string\')');
$db->query('INSERT INTO "Test Table47588" ("My Field") values(1), (2), (3)');
$rs = $db->query('SELECT * FROM "Test Table47588"');
var_dump($rs->fetchAll(PDO::FETCH_ASSOC));
echo "Done.\n";
?>
--CLEAN--
<?php
require __DIR__ . '/config.inc';
$db->exec('DROP TABLE IF EXISTS "Test Table47588"');
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
