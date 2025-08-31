--TEST--
Bug #65708 (dba functions cast $key param to string in-place, bypassing copy on write)
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
$name = 'bug65708.db';

$db = get_any_db($name);

$key = 1;
$copy = $key;

echo gettype($key)."\n";
echo gettype($copy)."\n";

dba_exists($key, $db);

echo gettype($key)."\n";
echo gettype($copy)."\n";

dba_close($db);

?>
--CLEAN--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
$db_name = 'bug65708.db';
cleanup_standard_db($db_name);
?>
--EXPECTF--
Using handler: "%s"
integer
integer
integer
integer
