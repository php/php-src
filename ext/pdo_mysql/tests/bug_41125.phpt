--TEST--
Bug #41125 (PDO mysql + quote() + prepare() can result in segfault)
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();

$db = MySQLPDOTest::factory();
$row = $db->query('SELECT VERSION() as _version')->fetch(PDO::FETCH_ASSOC);
$matches = array();
if (!preg_match('/^(\d+)\.(\d+)\.(\d+)/ismU', $row['_version'], $matches))
    die(sprintf("skip Cannot determine MySQL Server version\n"));

$version = $matches[1] * 10000 + $matches[2] * 100 + $matches[3];
if ($version < 40100)
    die(sprintf("skip Need MySQL Server 5.0.0+, found %d.%02d.%02d (%d)\n",
        $matches[1], $matches[2], $matches[3], $version));
?>
--FILE--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
$db = MySQLPDOTest::factory();
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);

// And now allow the evil to do his work
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 1);
$sql = "CREATE TABLE IF NOT EXISTS test_41125(id INT); INSERT INTO test_41125(id) VALUES (1); SELECT * FROM test_41125; INSERT INTO test_41125(id) VALUES (2); SELECT * FROM test_41125;";
$stmt = $db->query($sql);
do {
    var_dump($stmt->fetchAll());
} while ($stmt->nextRowset());

print "done!";
?>
--CLEAN--
<?php
require __DIR__ . '/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec("DROP TABLE IF EXISTS test_41125");
?>
--EXPECT--
array(0) {
}
array(0) {
}
array(1) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    [0]=>
    string(1) "1"
  }
}
array(0) {
}
array(2) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    [0]=>
    string(1) "1"
  }
  [1]=>
  array(2) {
    ["id"]=>
    string(1) "2"
    [0]=>
    string(1) "2"
  }
}
done!
