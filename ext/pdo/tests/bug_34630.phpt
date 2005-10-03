--TEST--
PDO Common: PHP Bug #34630: inserting streams as LOBs
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded('pdo')) die('skip');
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.dirname(__FILE__) . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
$db->exec('CREATE TABLE test (id int NOT NULL PRIMARY KEY, val VARCHAR(256))');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$fp = tmpfile();
fwrite($fp, "I am the LOB data");
rewind($fp);

$insert = $db->prepare("insert into test (id, val) values (1, ?)");
$insert->bindValue(1, $fp, PDO::PARAM_LOB);
$insert->execute();

print_r($db->query("SELECT * from test")->fetchAll(PDO::FETCH_ASSOC));

?>
--EXPECT--
Array
(
    [0] => Array
        (
            [id] => 1
            [val] => I am the LOB data
        )

)
