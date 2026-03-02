--TEST--
dba_fetch() legacy signature
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
$name = 'legacy_fetch_signature.db';

$db = get_any_db($name);

dba_insert("key1", "This is a test insert", $db);
echo dba_fetch("key1", 0, $db), \PHP_EOL, dba_fetch("key1", $db, 0), \PHP_EOL;

set_error_handler(function ($severity, $message, $file, $line) {
    throw new Exception($message);
});

try {
    dba_fetch("key1", 0, $db);
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
dba_close($db);

?>
--CLEAN--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
$db_name = 'legacy_fetch_signature.db';
cleanup_standard_db($db_name);
?>
--EXPECTF--
Using handler: "%s"

Deprecated: Calling dba_fetch() with $dba at the 3rd parameter is deprecated in %s on line %d
This is a test insert
This is a test insert
Calling dba_fetch() with $dba at the 3rd parameter is deprecated
