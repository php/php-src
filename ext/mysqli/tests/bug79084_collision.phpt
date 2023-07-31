--TEST--
Bug #79084 (mysqlnd may fetch wrong column indexes with MYSQLI_BOTH) - collision
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
require_once 'connect.inc';
$sql = "SELECT 11111 as `1`, 22222 as `2`";

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
array(3) {
  [0]=>
  string(5) "11111"
  [1]=>
  string(5) "11111"
  [2]=>
  string(5) "22222"
}
array(3) {
  [0]=>
  string(5) "11111"
  [1]=>
  string(5) "11111"
  [2]=>
  string(5) "22222"
}
array(3) {
  [0]=>
  string(5) "11111"
  [1]=>
  string(5) "11111"
  [2]=>
  string(5) "22222"
}
