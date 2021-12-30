--TEST--
Bug GH-7837 (large bigints may be truncated)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once("connect.inc");
if (strpos(mysqli_get_client_info(), "mysqlnd") === false) {
    die("skip requires mysqlnd");
}
require_once("skipifconnectfailure.inc");
?>
--FILE--
<?php
require_once("connect.inc");

$mysql = new mysqli($host, $user, $passwd, $db, $port, $socket);
$mysql->options(MYSQLI_OPT_INT_AND_FLOAT_NATIVE, true);
$mysql->query("DROP TABLE IF EXISTS test");
$mysql->query("CREATE TABLE test (`ubigint` bigint unsigned NOT NULL) ENGINE=InnoDB");
$mysql->query("INSERT INTO test (`ubigint`) VALUES (18446744073709551615)");
$mysql->query("INSERT INTO test (`ubigint`) VALUES (9223372036854775808)");
$mysql->query("INSERT INTO test (`ubigint`) VALUES (1)");
$result = $mysql->query("SELECT ubigint FROM test");
var_dump($result->fetch_all());
?>
--EXPECT--
array(3) {
  [0]=>
  array(1) {
    [0]=>
    string(20) "18446744073709551615"
  }
  [1]=>
  array(1) {
    [0]=>
    string(19) "9223372036854775808"
  }
  [2]=>
  array(1) {
    [0]=>
    int(1)
  }
}
