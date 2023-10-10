--TEST--
PDO Common: Bug #34630 (inserting streams as LOBs)
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

$driver = $db->getAttribute(PDO::ATTR_DRIVER_NAME);
$is_oci = $driver == 'oci';

if ($is_oci) {
    $db->exec('CREATE TABLE test34630 (id int NOT NULL PRIMARY KEY, val BLOB)');
} else {
    $db->exec('CREATE TABLE test34630 (id int NOT NULL PRIMARY KEY, val VARCHAR(256))');
}
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$fp = tmpfile();
fwrite($fp, "I am the LOB data");
rewind($fp);

if ($is_oci) {
    /* oracle is a bit different; you need to initiate a transaction otherwise
     * the empty blob will be committed implicitly when the statement is
     * executed */
    $db->beginTransaction();
    $insert = $db->prepare("insert into test34630 (id, val) values (1, EMPTY_BLOB()) RETURNING val INTO :blob");
} else {
    $insert = $db->prepare("insert into test34630 (id, val) values (1, :blob)");
}
$insert->bindValue(':blob', $fp, PDO::PARAM_LOB);
$insert->execute();
$insert = null;

$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);
var_dump($db->query("SELECT * from test34630")->fetchAll(PDO::FETCH_ASSOC));

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
$db->exec("DROP TABLE test34630");
?>
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["val"]=>
    string(17) "I am the LOB data"
  }
}
