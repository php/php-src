--TEST--
Bug #68199 (PDO::pgsqlGetNotify doesn't support NOTIFY payloads)
--EXTENSIONS--
pdo
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();

$db = PDOTest::factory();
if (version_compare($db->getAttribute(PDO::ATTR_SERVER_VERSION), '9.0.0') < 0) {
    die("skip Requires 9.0+");
}

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

// No parameters with payload, use default PDO::FETCH_NUM
$db->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_NUM);
$db->exec("NOTIFY notifies_phpt, 'payload'");
$notify = $db->pgsqlGetNotify();
var_dump(count($notify));
var_dump($notify[0]);
var_dump($notify[1] == $pid);
var_dump($notify[2]);

// No parameters with payload, use default PDO::FETCH_ASSOC
$db->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_ASSOC);
$db->exec("NOTIFY notifies_phpt, 'payload'");
$notify = $db->pgsqlGetNotify();
var_dump(count($notify));
var_dump($notify['message']);
var_dump($notify['pid'] == $pid);
var_dump($notify['payload']);

// Test PDO::FETCH_NUM as parameter with payload
$db->exec("NOTIFY notifies_phpt, 'payload'");
$notify = $db->pgsqlGetNotify(PDO::FETCH_NUM);
var_dump(count($notify));
var_dump($notify[0]);
var_dump($notify[1] == $pid);
var_dump($notify[2]);

// Test PDO::FETCH_ASSOC as parameter with payload
$db->exec("NOTIFY notifies_phpt, 'payload'");
$notify = $db->pgsqlGetNotify(PDO::FETCH_ASSOC);
var_dump(count($notify));
var_dump($notify['message']);
var_dump($notify['pid'] == $pid);
var_dump($notify['payload']);

// Test PDO::FETCH_BOTH as parameter with payload
$db->exec("NOTIFY notifies_phpt, 'payload'");
$notify = $db->pgsqlGetNotify(PDO::FETCH_BOTH);
var_dump(count($notify));
var_dump($notify['message']);
var_dump($notify['pid'] == $pid);
var_dump($notify['payload']);
var_dump($notify[0]);
var_dump($notify[1] == $pid);
var_dump($notify[2]);

// Verify that there are no notifies queued
var_dump($db->pgsqlGetNotify());

?>
--EXPECT--
bool(true)
bool(false)
bool(false)
int(3)
string(13) "notifies_phpt"
bool(true)
string(7) "payload"
int(3)
string(13) "notifies_phpt"
bool(true)
string(7) "payload"
int(3)
string(13) "notifies_phpt"
bool(true)
string(7) "payload"
int(3)
string(13) "notifies_phpt"
bool(true)
string(7) "payload"
int(6)
string(13) "notifies_phpt"
bool(true)
string(7) "payload"
string(13) "notifies_phpt"
bool(true)
string(7) "payload"
bool(false)
