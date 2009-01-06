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
  [u"affected_rows"]=>
  int(0)
  [u"client_info"]=>
  unicode(%d) "%s"
  [u"client_version"]=>
  int(%d)
  [u"connect_errno"]=>
  int(0)
  [u"connect_error"]=>
  unicode(0) ""
  [u"errno"]=>
  int(0)
  [u"error"]=>
  unicode(0) ""
  [u"field_count"]=>
  int(0)
  [u"host_info"]=>
  unicode(%d) "MySQL host info: %s via %s%s"
  [u"info"]=>
  NULL
  [u"insert_id"]=>
  int(0)
  [u"server_info"]=>
  unicode(%d) "%s"
  [u"server_version"]=>
  int(%d)
  [u"sqlstate"]=>
  unicode(5) "00000"
  [u"protocol_version"]=>
  int(10)
  [u"thread_id"]=>
  int(%d)
  [u"warning_count"]=>
  int(0)
}
object(mysqli)#%d (%d) {
  [u"affected_rows"]=>
  NULL
  [u"client_info"]=>
  unicode(%d) "%s"
  [u"client_version"]=>
  int(%d)
  [u"connect_errno"]=>
  int(0)
  [u"connect_error"]=>
  unicode(0) ""
  [u"errno"]=>
  int(0)
  [u"error"]=>
  unicode(0) ""
  [u"field_count"]=>
  NULL
  [u"host_info"]=>
  NULL
  [u"info"]=>
  NULL
  [u"insert_id"]=>
  NULL
  [u"server_info"]=>
  NULL
  [u"server_version"]=>
  NULL
  [u"sqlstate"]=>
  NULL
  [u"protocol_version"]=>
  NULL
  [u"thread_id"]=>
  NULL
  [u"warning_count"]=>
  NULL
}
object(mysqli_warning)#%d (%d) {
  [u"message"]=>
  unicode(25) "Column 'a' cannot be null"
  [u"sqlstate"]=>
  unicode(5) "HY000"
  [u"errno"]=>
  int(1048)
}
Done
