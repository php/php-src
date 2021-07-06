--TEST--
PDO Common: PDO::quote()
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
if (!strncasecmp(getenv('PDOTEST_DSN'), 'odbc', strlen('odbc'))) die('skip odbc driver doesn\'t have escape API, use prepared statements');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();

$unquoted = ' !"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~';

$quoted = $db->quote($unquoted);

$len = strlen($unquoted);

@$db->exec("DROP TABLE test");

$db->query("CREATE TABLE test (t char($len))");
$db->query("INSERT INTO test (t) VALUES($quoted)");

$stmt = $db->prepare('SELECT * from test');
$stmt->execute();

print_r($stmt->fetchAll(PDO::FETCH_ASSOC));

$db->exec("DROP TABLE test");

?>
--EXPECT--
Array
(
    [0] => Array
        (
            [t] =>  !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
        )

)
