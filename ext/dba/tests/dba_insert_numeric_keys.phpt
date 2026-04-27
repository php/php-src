--TEST--
dba_insert should correctly handle numeric array-like keys (php_dba_make_key)
--EXTENSIONS--
dba
--SKIPIF--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
check_skip_any();
?>
--FILE--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
$name = 'array_keys_db.db';

$db = get_any_db($name);

/* insert using numeric keys */
$a = [];
$a[1] = 'blah';
$a[0] = 'foo';
dba_insert($a, "abc", $db);

echo dba_fetch($a, $db) . PHP_EOL;

$a = [];
$a[0] = 'foo';
$a['bar'] = 'baz';

dba_insert($a, "mixed", $db);
echo dba_fetch($a, $db) . PHP_EOL;

$a = [];
$a[2] = 'abc';
$a[0] = 'foo';
dba_insert($a, "value", $db);
echo dba_fetch($a, $db) . PHP_EOL;

dba_close($db);
echo "OK\n";
?>
--CLEAN--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
$db_name = 'array_keys_db.db';
cleanup_standard_db($db_name);
?>
--EXPECTF--
Using handler: "%s"
abc
mixed
value
OK