--TEST--
PECL Bug #7976 (Calling stored procedure several times)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_mysql')) die('skip not loaded');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
$db = MySQLPDOTest::factory();

$row = $db->query('SELECT VERSION() as _version')->fetch(PDO::FETCH_ASSOC);
$matches = array();
if (!preg_match('/^(\d+)\.(\d+)\.(\d+)/ismU', $row['_version'], $matches))
    die(sprintf("skip Cannot determine MySQL Server version\n"));

$version = $matches[1] * 10000 + $matches[2] * 100 + $matches[3];
if ($version < 50000)
    die(sprintf("skip Need MySQL Server 5.0.0+, found %d.%02d.%02d (%d)\n",
        $matches[1], $matches[2], $matches[3], $version));
?>
--FILE--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
$db = MySQLPDOTest::factory();

function bug_pecl_7976($db) {

    $db->exec('DROP PROCEDURE IF EXISTS p');
    $db->exec('CREATE PROCEDURE p() BEGIN SELECT "1" AS _one; END;');

    $stmt = $db->query('CALL p()');
    var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
    $stmt->closeCursor();

    $stmt = $db->query('CALL p()');
    var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
    $stmt->closeCursor();

}

printf("Emulated...\n");
$db = MySQLPDOTest::factory();
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 1);
bug_pecl_7976($db);

printf("Native...\n");
$db = MySQLPDOTest::factory();
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 0);
bug_pecl_7976($db);

print "done!";
?>
--CLEAN--
<?php
require __DIR__ . '/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP PROCEDURE IF EXISTS p');
?>
--EXPECT--
Emulated...
array(1) {
  [0]=>
  array(1) {
    ["_one"]=>
    string(1) "1"
  }
}
array(1) {
  [0]=>
  array(1) {
    ["_one"]=>
    string(1) "1"
  }
}
Native...
array(1) {
  [0]=>
  array(1) {
    ["_one"]=>
    string(1) "1"
  }
}
array(1) {
  [0]=>
  array(1) {
    ["_one"]=>
    string(1) "1"
  }
}
done!
