--TEST--
PDO Common: PDO::quote()
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

$unquoted = ' !"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~';

$quoted = $db->quote($unquoted);

$db->query("CREATE TABLE test (t char(100))");
$db->query("INSERT INTO test (t) VALUES($quoted)");

$stmt = $db->prepare('SELECT * from test');
$stmt->execute();

print_r($stmt->fetchAll(PDO::FETCH_ASSOC));


?>
--EXPECT--
Array
(
    [0] => Array
        (
            [t] =>  !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
        )

)
