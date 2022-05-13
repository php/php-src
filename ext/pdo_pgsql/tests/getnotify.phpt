--TEST--
PDO PgSQL LISTEN/NOTIFY support
--EXTENSIONS--
pdo
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

// pgsqlGetPid should return something meaningful
$pid = $db->pgsqlGetPid();
var_dump($pid > 0);

// No listen, no notifies
var_dump($db->pgsqlGetNotify());

// Listen started, no notifies
$db->exec("LISTEN notifies_phpt");
var_dump($db->pgsqlGetNotify());

// No parameters, use default PDO::FETCH_NUM
$db->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_NUM);
$db->exec("NOTIFY notifies_phpt");
$notify = $db->pgsqlGetNotify();
var_dump(count($notify));
var_dump($notify[0]);
var_dump($notify[1] == $pid);

// No parameters, use default PDO::FETCH_ASSOC
$db->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_ASSOC);
$db->exec("NOTIFY notifies_phpt");
$notify = $db->pgsqlGetNotify();
var_dump(count($notify));
var_dump($notify['message']);
var_dump($notify['pid'] == $pid);

// Test PDO::FETCH_NUM as parameter
$db->exec("NOTIFY notifies_phpt");
$notify = $db->pgsqlGetNotify(PDO::FETCH_NUM);
var_dump(count($notify));
var_dump($notify[0]);
var_dump($notify[1] == $pid);

// Test PDO::FETCH_ASSOC as parameter
$db->exec("NOTIFY notifies_phpt");
$notify = $db->pgsqlGetNotify(PDO::FETCH_ASSOC);
var_dump(count($notify));
var_dump($notify['message']);
var_dump($notify['pid'] == $pid);

// Test PDO::FETCH_BOTH as parameter
$db->exec("NOTIFY notifies_phpt");
$notify = $db->pgsqlGetNotify(PDO::FETCH_BOTH);
var_dump(count($notify));
var_dump($notify['message']);
var_dump($notify['pid'] == $pid);
var_dump($notify[0]);
var_dump($notify[1] == $pid);

// Verify that there are no notifies queued
var_dump($db->pgsqlGetNotify());


// Test second parameter, should wait 2 seconds because no notify is queued
$t = microtime(1);
$notify = $db->pgsqlGetNotify(PDO::FETCH_ASSOC, 1000);
$diff = microtime(1) - $t;
var_dump($diff >= 1 || 1 - abs($diff) < .05);
var_dump($notify);

// Test second parameter, should return immediately because a notify is queued
$db->exec("NOTIFY notifies_phpt");
$t = microtime(1);
$notify = $db->pgsqlGetNotify(PDO::FETCH_ASSOC, 5000);
$diff = microtime(1) - $t;
var_dump($diff < 1 || abs(1 - abs($diff)) < .05);
var_dump(count($notify));

?>
--EXPECT--
bool(true)
bool(false)
bool(false)
int(2)
string(13) "notifies_phpt"
bool(true)
int(2)
string(13) "notifies_phpt"
bool(true)
int(2)
string(13) "notifies_phpt"
bool(true)
int(2)
string(13) "notifies_phpt"
bool(true)
int(4)
string(13) "notifies_phpt"
bool(true)
string(13) "notifies_phpt"
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
int(2)
