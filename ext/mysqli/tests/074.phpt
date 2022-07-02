--TEST--
mysqli_autocommit() tests
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php

    require_once("connect.inc");

    $mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

    var_dump($mysqli->autocommit(false));
    $result = $mysqli->query("SELECT @@autocommit");
    var_dump($result->fetch_row());

    var_dump($mysqli->autocommit(true));
    $result = $mysqli->query("SELECT @@autocommit");
    var_dump($result->fetch_row());

?>
--EXPECT--
bool(true)
array(1) {
  [0]=>
  string(1) "0"
}
bool(true)
array(1) {
  [0]=>
  string(1) "1"
}
