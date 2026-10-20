--TEST--
PDO Common: Attempting to initialize an opaque object via PDO::FETCH_CLASS
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();

$db->exec('CREATE TABLE pdo_fetch_class_opaque_object(id int NOT NULL PRIMARY KEY, path VARCHAR(10))');
$db->exec("INSERT INTO pdo_fetch_class_opaque_object VALUES(1, 'AA')");
$db->exec("INSERT INTO pdo_fetch_class_opaque_object VALUES(2, 'BB')");
$db->exec("INSERT INTO pdo_fetch_class_opaque_object VALUES(3, 'CC')");

$stmt = $db->prepare('SELECT path FROM pdo_fetch_class_opaque_object');
$stmt->execute();

var_dump($stmt->fetchAll(PDO::FETCH_CLASS, 'Directory', []));
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_fetch_class_opaque_object");
?>
--EXPECTF--
array(3) {
  [0]=>
  object(Directory)#%s (1) {
    ["path"]=>
    string(2) "AA"
    ["handle"]=>
    uninitialized(mixed)
  }
  [1]=>
  object(Directory)#%s (1) {
    ["path"]=>
    string(2) "BB"
    ["handle"]=>
    uninitialized(mixed)
  }
  [2]=>
  object(Directory)#%s (1) {
    ["path"]=>
    string(2) "CC"
    ["handle"]=>
    uninitialized(mixed)
  }
}
