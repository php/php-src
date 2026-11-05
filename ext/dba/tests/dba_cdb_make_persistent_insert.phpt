--TEST--
dba_popen() cdb_make handle inserted into in consecutive requests
--EXTENSIONS--
dba
--SKIPIF--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
check_skip('cdb_make');
if (PHP_OS_FAMILY === 'Windows') {
    die('skip SIGINT cannot shut down the CLI server on Windows');
}
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

$db_file = __DIR__ . '/dba_cdb_make_persistent_insert.db';

$args = ["-d", "extension_dir=" . ini_get("extension_dir"), "-d", "extension=" . (substr(PHP_OS, 0, 3) == "WIN" ? "php_" : "") . "dba." . PHP_SHLIB_SUFFIX];
if (php_ini_loaded_file()) {
    $args[] = "-c";
    $args[] = php_ini_loaded_file();
}
$info = php_cli_server_start('$db = dba_popen(' . var_export($db_file, true) . ', "n", "cdb_make"); var_dump(dba_insert("key" . $_GET["n"], "value" . $_GET["n"], $db));', 'router.php', $args);

for ($i = 1; $i <= 2; $i++) {
    echo "Request $i:\n";
    echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/?n=' . $i);
}

/* The persistent handle writes the cdb index when the server shuts down. */
proc_terminate($info->processHandle, 2);
for ($i = 0; $i < 100 && proc_get_status($info->processHandle)['running']; $i++) {
    usleep(50000);
}

$db = dba_open($db_file, 'r-', 'cdb');
var_dump(dba_fetch('key1', $db), dba_fetch('key2', $db));
dba_close($db);
?>
--CLEAN--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
cleanup_standard_db(__DIR__ . '/dba_cdb_make_persistent_insert.db');
?>
--EXPECT--
Request 1:
bool(true)
Request 2:
bool(true)
string(6) "value1"
string(6) "value2"
