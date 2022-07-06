--TEST--
Bug #33491 (extended mysqli class crashes when result is not object)
--INI--
error_reporting=1
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php

class DB extends mysqli
{
  public function query_single($query) {
    $result = parent::query($query);
    $result->fetch_row(); // <- Here be crash
  }
}

require_once("connect.inc");

// Segfault when using the DB class which extends mysqli
$DB = new DB($host, $user, $passwd, $db, $port, $socket);
$DB->query_single('SELECT DATE()');

?>
--EXPECTF--
Fatal error: Uncaught Error: Call to a member function fetch_row() on bool in %sbug33491.php:%d
Stack trace:
#0 %s(%d): DB->query_single('SELECT DATE()')
#1 {main}
  thrown in %sbug33491.php on line %d
