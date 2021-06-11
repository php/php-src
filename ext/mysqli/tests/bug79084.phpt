--TEST--
Bug #79084 (mysqlnd may fetch wrong column indexes with MYSQLI_BOTH)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
require_once('connect.inc');
$sql = "SELECT 0 as `2007`, 0 as `2008`, 0 as `2020`";

// unbuffered
$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
$link->real_query($sql);
$res = $link->use_result();
$row = $res->fetch_array();
var_dump($row);
$link->close();

// buffered
ini_set('mysqlnd.fetch_data_copy', false);
$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
$res = $link->query($sql);
$row = $res->fetch_array();
var_dump($row);
$link->close();

// buffered copies
ini_set('mysqlnd.fetch_data_copy', true);
$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
$res = $link->query($sql);
$row = $res->fetch_array();
var_dump($row);
$link->close();
?>
--EXPECT--
array(6) {
  [0]=>
  string(1) "0"
  [2007]=>
  string(1) "0"
  [1]=>
  string(1) "0"
  [2008]=>
  string(1) "0"
  [2]=>
  string(1) "0"
  [2020]=>
  string(1) "0"
}
array(6) {
  [0]=>
  string(1) "0"
  [2007]=>
  string(1) "0"
  [1]=>
  string(1) "0"
  [2008]=>
  string(1) "0"
  [2]=>
  string(1) "0"
  [2020]=>
  string(1) "0"
}
array(6) {
  [0]=>
  string(1) "0"
  [2007]=>
  string(1) "0"
  [1]=>
  string(1) "0"
  [2008]=>
  string(1) "0"
  [2]=>
  string(1) "0"
  [2020]=>
  string(1) "0"
}
