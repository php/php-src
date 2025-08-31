--TEST--
PDO::quote() must accept empty string for drivers which support this feature
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

$pdo = PDOTest::factory();
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

try {
    $result = $pdo->quote('');
    if (!is_string($result)) {
        var_dump($result);
    }
} catch (\PDOException) {
    // Do nothing as quoting is not supported with this driver
}
?>
DONE

--EXPECT--
DONE
