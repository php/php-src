--TEST--
SQLite3 memory limit test
--EXTENSIONS--
sqlite3
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") { die("skip requires zmm"); }
?>
--INI--
memory_limit=10M
--FILE--
<?php
require __DIR__ . '/new_db.inc';

var_dump($db->exec('CREATE TABLE test (foo TEXT)'));

var_dump($db->exec('INSERT INTO test (foo) VALUES (\'short_text\')'));
$mBefore = memory_get_usage();
var_dump($db->exec('INSERT INTO test (foo) VALUES (\'' . bin2hex(random_bytes(2 * 1024 * 1024)) . '\')'));
$mDiff = memory_get_usage() - $mBefore;
if ($mDiff < 3 * 1024 * 1024) {
    echo "UNEXPECTED: 4 MB of data inserted, but only " . round($mDiff / (1024 ** 2), 2) . " MB of memory counted\n";
}

$db = null; // close DB

$mDiff = memory_get_usage() - $mBefore;
if ($mDiff > 1 * 1024 * 1024) {
    echo "UNEXPECTED: Database purged, but " . round($mDiff / (1024 ** 2), 2) . " MB of memory remained allocated\n";
}

require __DIR__ . '/new_db.inc';

var_dump($db->exec('CREATE TABLE test (foo TEXT)'));

var_dump($db->exec('INSERT INTO test (foo) VALUES (\'' . bin2hex(random_bytes(10 * 1024 * 1024)) . '\')'));
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
Fatal error: Allowed memory size of 10485760 bytes exhausted%s(tried to allocate %d bytes) in %s on line %d
