--TEST--
Bug #66043 (Segfault calling bind_param() on mysqli)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
require 'connect.inc';
if (!$db = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
    printf("[001] Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
}

if (!$db->query("DROP TABLE IF EXISTS test")) {
    printf("[002] [%d] %s\n", mysqli_errno($db), mysqli_error($db));
    die();
}

if (!$db->query("CREATE TABLE test(str TEXT)")) {
    printf("[003] [%d] %s\n", mysqli_errno($db), mysqli_error($db));
    die();
}

if (!$db->query("INSERT INTO test(str) VALUES ('Test')")) {
    printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    die();
}

$stmt = $db->stmt_init();
if (!$stmt->prepare("SELECT str FROM test")) {
    printf("[004] [%d] %s\n", mysqli_errno($db), mysqli_error($db));
    die();
}

$stmt->execute();
$stmt->bind_result($testArg);
echo "Okey";
?>
--CLEAN--
<?php
require_once("clean_table.inc");
?>
--EXPECT--
Okey
