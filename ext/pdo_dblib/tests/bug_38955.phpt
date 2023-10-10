--TEST--
 PDO_DBLIB driver does not support transactions
--EXTENSIONS--
pdo_dblib
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
?>
--FILE--
<?php
require __DIR__ . '/config.inc';

/*We see these rows */
$db->query("CREATE table test38955(val int)");
$db->beginTransaction();
$db->query("INSERT INTO test38955(val) values(1)");
$db->query("INSERT INTO test38955(val) values(2)");
$db->query("INSERT INTO test38955(val) values(3)");
$db->query("INSERT INTO test38955(val) values(4)");
$db->commit();

/*We don't see these rows */
$db->beginTransaction();
$db->query("INSERT INTO test38955(val) values(5)");
$db->query("INSERT INTO test38955(val) values(6)");
$db->query("INSERT INTO test38955(val) values(7)");
$db->query("INSERT INTO test38955(val) values(8)");
$db->rollback();

$rs = $db->query("SELECT * FROM test38955");
$rows = $rs->fetchAll(PDO::FETCH_ASSOC);
var_dump($rows);
?>
--CLEAN--
<?php
require __DIR__ . '/config.inc';
$db->exec("DROP TABLE IF EXISTS test38955");
?>
--EXPECT--
array(4) {
  [0]=>
  array(1) {
    ["val"]=>
    int(1)
  }
  [1]=>
  array(1) {
    ["val"]=>
    int(2)
  }
  [2]=>
  array(1) {
    ["val"]=>
    int(3)
  }
  [3]=>
  array(1) {
    ["val"]=>
    int(4)
  }
}
