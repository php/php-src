--TEST--
PDO Common: Bug #40285 (The prepare parser goes into an infinite loop on ': or ":)
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

$db->exec('CREATE TABLE test40285 (field1 VARCHAR(32), field2 VARCHAR(32), field3 VARCHAR(32), field4 INT)');

$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);
$s = $db->prepare("INSERT INTO test40285 VALUES( ':id', 'name', 'section', 22)" );
$s->execute();

echo "Done\n";
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
$db->exec("DROP TABLE test40285");
?>
--EXPECT--
Done
