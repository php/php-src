--TEST--
Bug #71863 Segfault when EXPLAIN with "Unknown Column" Error
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
require_once("connect.inc");
if (!$IS_MYSQLND) {
    die("skip mysqlnd only test");
}
?>
--FILE--
<?php
require_once("connect.inc");

$req = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

// create db and table for test
mysqli_query($req, "DROP TABLE IF EXISTS test_bug_71863") or die(mysqli_error($req));
mysqli_query($req, "CREATE TABLE test_bug_71863 (id INT UNSIGNED NOT NULL DEFAULT 0)") or die(mysqli_error($req));

// segfault if EXPLAIN + "Unknown column" error
mysqli_query($req, "EXPLAIN SELECT `id` FROM `test_bug_71863` WHERE `owner_id` = '2' AND `object_id` = '1' AND type = '0'") or die(mysqli_error($req)."\n");

?>
--CLEAN--
<?php
require_once("connect.inc");
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
    printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
if (!mysqli_query($link, "DROP TABLE IF EXISTS test_bug_71863"))
    printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));
mysqli_close($link);
?>
--EXPECTF--
%AUnknown column 'owner_id' in 'where clause'
