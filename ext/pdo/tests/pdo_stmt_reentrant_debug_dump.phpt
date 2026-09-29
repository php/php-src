--TEST--
PDO Common: PDOStatement rejects reentrant binding from a debug output handler
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if ($dir === false) {
    die('skip no driver');
}
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) {
    putenv('REDIR_TEST_DIR=' . __DIR__ . '/../../pdo/tests/');
}
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
$db->exec('CREATE TABLE pdo_reentrant_debug_dump (val INT)');
$db->exec('INSERT INTO pdo_reentrant_debug_dump VALUES (1)');
$statement = $db->prepare('SELECT val FROM pdo_reentrant_debug_dump WHERE val = ?');
$statement->bindValue(1, 1, PDO::PARAM_INT);

$called = false;
$messages = [];
$output = '';
ob_start(static function (string $buffer) use ($statement, &$called, &$messages, &$output): string {
    $output .= $buffer;
    if (!$called && str_contains($output, 'Key: Position')) {
        $called = true;
        try {
            $statement->bindValue(1, 2, PDO::PARAM_INT);
        } catch (Throwable $e) {
            $messages[] = $e::class . ': ' . $e->getMessage();
        }
    }
    return '';
}, 1);
$result = $statement->debugDumpParams();
ob_end_clean();

echo implode("\n", $messages), "\n";
var_dump($result);
var_dump($statement->execute());
var_dump($statement->fetchColumn());
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, 'pdo_reentrant_debug_dump');
?>
--EXPECT--
Error: Cannot perform another operation on this PDOStatement while an operation is in progress
NULL
bool(true)
string(1) "1"
