--TEST--
GH-22349 (Heap over-read fetching a long column past the internal buffer)
--EXTENSIONS--
pdo_odbc
--SKIPIF--
<?php
require 'ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory('ext/pdo_odbc/tests/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);

$db->exec('DROP TABLE test_gh22349');
if (false === $db->exec('CREATE TABLE test_gh22349 (data text)')
    && false === $db->exec('CREATE TABLE test_gh22349 (data CLOB)')
    && false === $db->exec('CREATE TABLE test_gh22349 (data longtext)')) {
    die("BORK: no large text column type available here: " . implode(", ", $db->errorInfo()) . "\n");
}

$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

// The driver fetches a long column into an internal buffer of roughly one
// memory page and reassembles the remainder. Exercise values that span and
// exceed that buffer so the seeded length must match the bytes present.
foreach ([4096, 8192, 65536] as $len) {
    $db->exec('DELETE FROM test_gh22349');
    $text = str_repeat('A', $len);
    $db->exec("INSERT INTO test_gh22349 VALUES ('$text')");
    $got = $db->query('SELECT data FROM test_gh22349')->fetchColumn();
    printf("%d: %s\n", $len, ($got === $text) ? 'ok' : ('MISMATCH len=' . strlen($got)));
}
?>
--CLEAN--
<?php
require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory('ext/pdo_odbc/tests/common.phpt');
$db->exec('DROP TABLE test_gh22349');
?>
--EXPECT--
4096: ok
8192: ok
65536: ok
