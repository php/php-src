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
		var_dump(new mysqli_warning($mysql));
	} 
} 

$db = new DbConnection(); 
$db->connect();

echo "Done\n";
?>
--EXPECTF--	
object(mysqli)#%d (%d) {
  ["affected_rows"]=>
  int(0)
  ["client_info"]=>
  string(%d) "%s"
  ["client_version"]=>
  int(%d)
  ["connect_errno"]=>
  int(0)
  ["connect_error"]=>
  string(0) ""
  ["errno"]=>
  int(0)
  ["error"]=>
  string(0) ""
  ["field_count"]=>
  int(0)
  ["host_info"]=>
  string(%d) "MySQL host info: %s via %s%s"
  ["info"]=>
  NULL
  ["insert_id"]=>
  int(0)
  ["server_info"]=>
  string(%d) "%s"
  ["server_version"]=>
  int(%d)
  ["sqlstate"]=>
  string(5) "00000"
  ["protocol_version"]=>
  int(10)
  ["thread_id"]=>
  int(%d)
  ["warning_count"]=>
  int(0)
}
object(mysqli)#%d (%d) {
  ["affected_rows"]=>
  NULL
  ["client_info"]=>
  string(%d) "%s"
  ["client_version"]=>
  int(%d)
  ["connect_errno"]=>
  int(0)
  ["connect_error"]=>
  string(0) ""
  ["errno"]=>
  int(0)
  ["error"]=>
  string(0) ""
  ["field_count"]=>
  NULL
  ["host_info"]=>
  NULL
  ["info"]=>
  NULL
  ["insert_id"]=>
  NULL
  ["server_info"]=>
  NULL
  ["server_version"]=>
  NULL
  ["sqlstate"]=>
  NULL
  ["protocol_version"]=>
  NULL
  ["thread_id"]=>
  NULL
  ["warning_count"]=>
  NULL
}
object(mysqli_warning)#%d (%d) {
  ["message"]=>
  string(25) "Column 'a' cannot be null"
  ["sqlstate"]=>
  string(5) "HY000"
  ["errno"]=>
  int(1048)
}
Done
