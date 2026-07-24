--TEST--
mysqli_pconnect() - COM_RESET_CONNECTION clears session state
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
require_once 'connect.inc';

$host = 'p:' . $host;

$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
if (!$link) {
    printf("[001] Cannot connect\n");
}

mysqli_query($link, "SET @test_var = 42");
mysqli_query($link, "CREATE TEMPORARY TABLE test_reset_tmp (id INT)");
mysqli_query($link, "INSERT INTO test_reset_tmp VALUES (1)");

$res = mysqli_query($link, "SELECT @test_var AS v");
$row = mysqli_fetch_assoc($res);
if ($row['v'] !== '42') {
    printf("[002] Expected 42, got %s\n", $row['v']);
}
mysqli_free_result($res);

$thread_id = mysqli_thread_id($link);
mysqli_close($link);

$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
if (!$link) {
    printf("[003] Cannot reconnect\n");
}

if (mysqli_thread_id($link) !== $thread_id) {
    printf("[004] Got a different connection, persistent reuse did not happen\n");
}

$res = mysqli_query($link, "SELECT @test_var AS v");
$row = mysqli_fetch_assoc($res);
if ($row['v'] !== null) {
    printf("[005] Expected NULL after reset, got %s\n", $row['v']);
}
mysqli_free_result($res);

/* SHOW TABLES never lists temporary tables, so probe by selecting from it */
if (false !== @mysqli_query($link, 'SELECT COUNT(*) FROM test_reset_tmp')) {
    printf("[006] Temporary table should not exist after reset\n");
}

mysqli_close($link);

echo "done!";
?>
--EXPECT--
done!
