--TEST--
Bug #44707 (The MySQL PDO driver resets variable content after bindParam on tinyint field)
--EXTENSIONS--
pdo
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

MySQLPDOTest::skip();

$db = MySQLPDOTest::factory();
$stmt = $db->query('SELECT VERSION() as _version');
$row = $stmt->fetch(PDO::FETCH_ASSOC);
$matches = array();
if (!preg_match('/^(\d+)\.(\d+)\.(\d+)/ismU', $row['_version'], $matches))
    die(sprintf("skip Cannot determine MySQL Server version\n"));

$version = $matches[1] * 10000 + $matches[2] * 100 + $matches[3];
if ($version < 41000)
    die(sprintf("skip Will work different with MySQL Server < 4.1.0, found %d.%02d.%02d (%d)\n",
        $matches[1], $matches[2], $matches[3], $version));
?>
--FILE--
<?php
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');

function bug_44707($db) {

    $db->exec('DROP TABLE IF EXISTS test');
    $db->exec('CREATE TABLE test(id INT, mybool TINYINT)');

    $id = 1;
    $mybool = false;
    var_dump($mybool);

    $stmt = $db->prepare('INSERT INTO test(id, mybool) VALUES (?, ?)');
    $stmt->bindParam(1, $id);
    $stmt->bindParam(2, $mybool, PDO::PARAM_BOOL);
    var_dump($mybool);

    $stmt->execute();
    var_dump($mybool);

    $stmt = $db->query('SELECT * FROM test');
    var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

    $stmt = $db->prepare('INSERT INTO test(id, mybool) VALUES (?, ?)');
    $stmt->bindParam(1, $id);
    // INT and integer work well together
    $stmt->bindParam(2, $mybool, PDO::PARAM_INT);
    $stmt->execute();

    $stmt = $db->query('SELECT * FROM test');
    var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

}


/*
// This is beyond the control of the driver... - the driver never gets in touch with bound values
print "Emulated Prepared Statements\n";
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 1);
bug_44707($db);
*/

print "Native Prepared Statements\n";
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 0);
bug_44707($db);

print "done!";
?>
--EXPECT--
Native Prepared Statements
bool(false)
bool(false)
bool(false)
array(1) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["mybool"]=>
    string(1) "0"
  }
}
array(2) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["mybool"]=>
    string(1) "0"
  }
  [1]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["mybool"]=>
    string(1) "0"
  }
}
done!
