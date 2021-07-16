--TEST--
PDO Common: TypeError when binding an object as a boolean which cannot be converted
--EXTENSIONS--
pdo
gmp
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

$db->exec('CREATE TABLE test(id int NOT NULL PRIMARY KEY, val boolean)');
$db->exec('INSERT INTO test VALUES(1, TRUE)');
$db->exec('INSERT INTO test VALUES(2, FALSE)');

$gmp = gmp_init('20', 10);

$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);
$s = $db->prepare('SELECT id FROM test WHERE val = :bval');
$s->bindValue(':bval', $gmp, PDO::PARAM_BOOL);

try {
    $s->execute();
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
?>
==DONE==
--EXPECT--
Object of class GMP could not be converted to bool
==DONE==
