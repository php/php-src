--TEST--
DBA with invalid array key
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
$name = 'array_keys_errors.db';

$db = get_any_db($name);

try {
    dba_insert([], "Content String 1", $db);
} catch (\Error $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
try {
    dba_insert(["a", "b", "c"], "Content String 2", $db);
} catch (\Error $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

/* Use an object */
$o = new stdClass();
try {
    var_dump(dba_insert([$o, 'obj'], 'Test', $db));
} catch (\Error $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
try {
    var_dump(dba_insert(['group', $o], 'Test', $db));
} catch (\Error $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

dba_close($db);

?>
--CLEAN--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
$db_name = 'array_keys_errors.db';
cleanup_standard_db($db_name);
?>
--EXPECTF--
Using handler: "%s"
Error: dba_insert(): Argument #1 ($key) must have exactly two elements: "key" and "name"
Error: dba_insert(): Argument #1 ($key) must have exactly two elements: "key" and "name"
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
