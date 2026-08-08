--TEST--
mysqli: reusing a persistent connection resets its session state (COM_RESET_CONNECTION)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
// Reusing a pooled persistent connection sends COM_RESET_CONNECTION, which must
// wipe all existing session state.

require_once 'connect.inc';

$host = 'p:' . $host;

// Establish a persistent handle with some session state.
$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

if (!$link) {
    printf("[001] Cannot connect\n");
}

$thread_id = mysqli_thread_id($link);

// Pick a charset the connection does not already use, so the change is observable.
$default = mysqli_character_set_name($link);

if ($default === 'latin1') {
    printf("[002] Test needs a default charset other than latin1, got %s\n", $default);
}

mysqli_query($link, "SET @test_var = 42");
mysqli_query($link, "CREATE TEMPORARY TABLE test_reset_tmp (id INT)");

mysqli_query($link, "DROP TABLE IF EXISTS test_reset_trx");
mysqli_query($link, "CREATE TABLE test_reset_trx (id INT) ENGINE=InnoDB");

mysqli_begin_transaction($link);
mysqli_query($link, "INSERT INTO test_reset_trx VALUES (1)");

// Change the charset after connecting, which is not part of the connection config.
mysqli_set_charset($link, 'latin1');

// Return the connection to the pool.
mysqli_close($link);

// Reopen the same persistent connection, which should be reset.
$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

// Compare the thread IDs to ensure the connection was reused.
if (mysqli_thread_id($link) !== $thread_id) {
    printf("[003] Expected the pooled connection to be reused\n");
}

// Check the session variable to ensure it is no longer present.
$v = mysqli_query($link, "SELECT @test_var")->fetch_row()[0];

if ($v !== null) {
    printf("[004] User variable should be reset, got %s\n", var_export($v, true));
}

// Check the temporary table to ensure it is no longer present.
if (@mysqli_query($link, "SELECT 1 FROM test_reset_tmp") !== false) {
    printf("[005] Temporary table should not exist after reset\n");
}

// Check to see we are no longer in a transaction.
$rows = mysqli_query($link, "SELECT COUNT(*) FROM test_reset_trx")->fetch_row()[0];

if ($rows != 0) {
    printf("[006] Transaction should have been rolled back, found %d row(s)\n", $rows);
}

// A charset set after connecting is not part of the config, so it is not restored.
if (mysqli_character_set_name($link) !== $default) {
    printf("[007] Charset should be reset to %s, got %s\n", $default, mysqli_character_set_name($link));
}

// mysqlnd's cached charset (used for escaping) must still agree with the server.
$server = mysqli_query($link, "SELECT @@session.character_set_connection")->fetch_row()[0];

if (mysqli_character_set_name($link) !== $server) {
    printf("[008] Client charset %s disagrees with server %s\n", mysqli_character_set_name($link), $server);
}

// Clean up the non-temporary table.
mysqli_query($link, "DROP TABLE IF EXISTS test_reset_trx");
mysqli_close($link);

echo "done!";
?>
--EXPECT--
done!
