--TEST--
dba_popen() flatfile handle iterated in consecutive requests
--EXTENSIONS--
dba
--SKIPIF--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
check_skip('flatfile');
if (!file_exists(__DIR__ . "/../../../sapi/cli/tests/php_cli_server.inc")) {
    echo "skip sapi/cli/tests/php_cli_server.inc required but not found";
}
?>
--CONFLICTS--
server
--ENV--
USE_ZEND_ALLOC=0
USE_TRACKED_ALLOC=1
--FILE--
<?php
include __DIR__ . "/../../../sapi/cli/tests/php_cli_server.inc";

$db_file = __DIR__ . '/dba_flatfile_persistent_iteration.db';
$db = dba_open($db_file, 'n', 'flatfile');
dba_insert('key1', 'value1', $db);
dba_insert('key2', 'value2', $db);
dba_close($db);

$args = ["-d", "extension_dir=" . ini_get("extension_dir"), "-d", "extension=" . (substr(PHP_OS, 0, 3) == "WIN" ? "php_" : "") . "dba." . PHP_SHLIB_SUFFIX];
if (php_ini_loaded_file()) {
    $args[] = "-c";
    $args[] = php_ini_loaded_file();
}
php_cli_server_start('$db = dba_popen(' . var_export($db_file, true) . ', "r", "flatfile"); var_dump(dba_firstkey($db), dba_nextkey($db));', 'router.php', $args);

for ($i = 1; $i <= 2; $i++) {
    echo "Request $i:\n";
    echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/');
}
?>
--CLEAN--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
cleanup_standard_db(__DIR__ . '/dba_flatfile_persistent_iteration.db');
?>
--EXPECT--
Request 1:
string(4) "key1"
string(4) "key2"
Request 2:
string(4) "key1"
string(4) "key2"
