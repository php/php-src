--TEST--
PDO Common: Bug #72788 (Invalid memory access when using persistent PDO connection)
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

putenv("PDOTEST_ATTR=" . serialize(array(PDO::ATTR_PERSISTENT => true)));

function test() {
    $db = PDOTest::factory('PDO', false);
    $stmt = @$db->query("SELECT 1 FROM TABLE_DOES_NOT_EXIST");
    if ($stmt === false) {
        echo "Statement failed as expected\n";
    }
}

test();
test();
echo "Done";
?>
--EXPECT--
Statement failed as expected
Statement failed as expected
Done
