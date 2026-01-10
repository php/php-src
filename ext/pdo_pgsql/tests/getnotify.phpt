--TEST--
PDO PgSQL LISTEN/NOTIFY support
--EXTENSIONS--
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
$db->exec("LISTEN channel_getnotify");
var_dump($db->pgsqlGetNotify());

// No parameters, use default PDO::FETCH_NUM
$db->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_NUM);
$db->exec("NOTIFY channel_getnotify");
$notify = $db->pgsqlGetNotify();
var_dump(count($notify));
var_dump($notify[0]);
var_dump($notify[1] == $pid);

// No parameters, use default PDO::FETCH_ASSOC
$db->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_ASSOC);
$db->exec("NOTIFY channel_getnotify");
$notify = $db->pgsqlGetNotify();
var_dump(count($notify));
var_dump($notify['message']);
var_dump($notify['pid'] == $pid);

// Test PDO::FETCH_NUM as parameter
$db->exec("NOTIFY channel_getnotify");
$notify = $db->pgsqlGetNotify(PDO::FETCH_NUM);
var_dump(count($notify));
var_dump($notify[0]);
var_dump($notify[1] == $pid);

// Test PDO::FETCH_ASSOC as parameter
$db->exec("NOTIFY channel_getnotify");
$notify = $db->pgsqlGetNotify(PDO::FETCH_ASSOC);
var_dump(count($notify));
var_dump($notify['message']);
var_dump($notify['pid'] == $pid);

// Test PDO::FETCH_BOTH as parameter
$db->exec("NOTIFY channel_getnotify");
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
$db->exec("NOTIFY channel_getnotify");
$t = microtime(1);
$notify = $db->pgsqlGetNotify(PDO::FETCH_ASSOC, 5000);
$diff = microtime(1) - $t;
var_dump($diff < 1 || abs(1 - abs($diff)) < .05);
var_dump(count($notify));

?>
--EXPECTF--
Deprecated: Method PDO::pgsqlGetPid() is deprecated since 8.5, use Pdo\Pgsql::getPid() instead in %s on line %d
bool(true)

Deprecated: Method PDO::pgsqlGetNotify() is deprecated since 8.5, use Pdo\Pgsql::getNotify() instead in %s on line %d
bool(false)

Deprecated: Method PDO::pgsqlGetNotify() is deprecated since 8.5, use Pdo\Pgsql::getNotify() instead in %s on line %d
bool(false)

Deprecated: Method PDO::pgsqlGetNotify() is deprecated since 8.5, use Pdo\Pgsql::getNotify() instead in %s on line %d
int(2)
string(17) "channel_getnotify"
bool(true)

Deprecated: Method PDO::pgsqlGetNotify() is deprecated since 8.5, use Pdo\Pgsql::getNotify() instead in %s on line %d
int(2)
string(17) "channel_getnotify"
bool(true)

Deprecated: Method PDO::pgsqlGetNotify() is deprecated since 8.5, use Pdo\Pgsql::getNotify() instead in %s on line %d
int(2)
string(17) "channel_getnotify"
bool(true)

Deprecated: Method PDO::pgsqlGetNotify() is deprecated since 8.5, use Pdo\Pgsql::getNotify() instead in %s on line %d
int(2)
string(17) "channel_getnotify"
bool(true)

Deprecated: Method PDO::pgsqlGetNotify() is deprecated since 8.5, use Pdo\Pgsql::getNotify() instead in %s on line %d
int(4)
string(17) "channel_getnotify"
bool(true)
string(17) "channel_getnotify"
bool(true)

Deprecated: Method PDO::pgsqlGetNotify() is deprecated since 8.5, use Pdo\Pgsql::getNotify() instead in %s on line %d
bool(false)

Deprecated: Method PDO::pgsqlGetNotify() is deprecated since 8.5, use Pdo\Pgsql::getNotify() instead in %s on line %d
bool(true)
bool(false)

Deprecated: Method PDO::pgsqlGetNotify() is deprecated since 8.5, use Pdo\Pgsql::getNotify() instead in %s on line %d
bool(true)
int(2)
