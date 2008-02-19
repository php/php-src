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
		var_dump($link);
		
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
object(mysqli)#%d (0) {
}
object(mysqli)#%d (0) {
}
object(mysqli_warning)#%d (0) {
}
Done
