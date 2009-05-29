--TEST--
Bug #34810 (mysqli::init() and others use wrong $this pointer without checks)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php

class DbConnection {
	public function connect() {
		include "connect.inc";

		$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket);
		var_dump($link);

		$link = mysqli_init();
		/* @ is to supress 'Property access is not allowed yet' */
		@var_dump($link);

		$mysql = new mysqli($host, $user, $passwd, $db, $port, $socket);
		$mysql->query("DROP TABLE IF EXISTS test_warnings");
		$mysql->query("CREATE TABLE test_warnings (a int not null)");
		$mysql->query("SET sql_mode=''");
		$mysql->query("INSERT INTO test_warnings VALUES (1),(2),(NULL)");
		var_dump($mysql->get_warnings());
	}
}

$db = new DbConnection();
$db->connect();

echo "Done\n";
?>
--EXPECTF--
object(mysqli)#%d (%d) {
  [%u|b%"affected_rows"]=>
  int(0)
  [%u|b%"client_info"]=>
  %unicode|string%(%d) "%s"
  [%u|b%"client_version"]=>
  int(%d)
  [%u|b%"connect_errno"]=>
  int(0)
  [%u|b%"connect_error"]=>
  %unicode|string%(0) ""
  [%u|b%"errno"]=>
  int(0)
  [%u|b%"error"]=>
  %unicode|string%(0) ""
  [%u|b%"field_count"]=>
  int(0)
  [%u|b%"host_info"]=>
  %unicode|string%(%d) "%s"
  [%u|b%"info"]=>
  NULL
  [%u|b%"insert_id"]=>
  int(0)
  [%u|b%"server_info"]=>
  %unicode|string%(%d) "%s"
  [%u|b%"server_version"]=>
  int(%d)
  [%u|b%"sqlstate"]=>
  %unicode|string%(5) "00000"
  [%u|b%"protocol_version"]=>
  int(10)
  [%u|b%"thread_id"]=>
  int(%d)
  [%u|b%"warning_count"]=>
  int(0)
}
object(mysqli)#%d (%d) {
  [%u|b%"affected_rows"]=>
  NULL
  [%u|b%"client_info"]=>
  %unicode|string%(%d) "%s"
  [%u|b%"client_version"]=>
  int(%d)
  [%u|b%"connect_errno"]=>
  int(0)
  [%u|b%"connect_error"]=>
  %unicode|string%(0) ""
  [%u|b%"errno"]=>
  int(0)
  [%u|b%"error"]=>
  %unicode|string%(0) ""
  [%u|b%"field_count"]=>
  NULL
  [%u|b%"host_info"]=>
  NULL
  [%u|b%"info"]=>
  NULL
  [%u|b%"insert_id"]=>
  NULL
  [%u|b%"server_info"]=>
  NULL
  [%u|b%"server_version"]=>
  NULL
  [%u|b%"sqlstate"]=>
  NULL
  [%u|b%"protocol_version"]=>
  NULL
  [%u|b%"thread_id"]=>
  NULL
  [%u|b%"warning_count"]=>
  NULL
}
object(mysqli_warning)#%d (%d) {
  [%u|b%"message"]=>
  %unicode|string%(25) "Column 'a' cannot be null"
  [%u|b%"sqlstate"]=>
  %unicode|string%(5) "HY000"
  [%u|b%"errno"]=>
  int(1048)
}
Done
