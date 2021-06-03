--TEST--
PDO_sqlite: Testing memory limit
--EXTENSIONS--
pdo_sqlite
--SKIPIF--
<?php



if (true) { die("skip ext/pdo_sqlite until ext/sqlite3 is fully working"); }



if (getenv("USE_ZEND_ALLOC") === "0") { die("skip requires zmm"); }
?>
--INI--
memory_limit=10M
--FILE--
<?php
$db = new PDO('sqlite::memory:');

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

$db = new PDO('sqlite::memory:');

var_dump($db->exec('CREATE TABLE test (foo TEXT)'));

var_dump($db->exec('INSERT INTO test (foo) VALUES (\'' . bin2hex(random_bytes(10 * 1024 * 1024)) . '\')'));
?>
--EXPECTF--
int(0)
int(1)
int(1)
int(0)
Fatal error: Allowed memory size of 10485760 bytes exhausted%s(tried to allocate %d bytes) in %s on line %d
